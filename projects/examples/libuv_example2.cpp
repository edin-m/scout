// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <set>
#include <memory>
#include <string>
#include <stdexcept>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "uv.h"

#include <google/protobuf/message.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "person/person.pb.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


typedef int TcpConnectionId;





template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}


template <class T>
class IdPool
{
 public:
  IdPool(T initial = 1) : id_(initial) {}

  T Next()
  {
    if (free_.size() > 0) {
      T id = free_.front();
      free_.pop_front();
      return id;
    }

    return id_++;
  }

  void Free(T id)
  {
    free_.push_back(id);
  }
 private:
  T id_;
  std::deque<T> free_;
};

using IntIdPool = IdPool<int>;







class Timer {
public:
    using TimeoutCb = std::function<void()>;

    Timer(uv_loop_t* loop_, long milis_, bool is_repeated_ = false)
        : loop(loop_), milis(milis_), is_repeated(is_repeated_)
    {
        timer.data = this;
        uv_timer_init(loop, &timer);
    }

    ~Timer() {
        Stop();
    }

    void OnTimeout(TimeoutCb f) { on_timeout = f; }

    void Start() {
        uint64_t repeat = is_repeated ? milis : 0;
        uv_timer_start(&timer, &Timer::OnTimerCb, milis, repeat);
    }

    void Stop() {
        uv_timer_stop(&timer);
    }

private:
    uv_loop_t* loop;
    uv_timer_t timer;
    long milis;
    bool is_repeated = false;

    TimeoutCb on_timeout;

    static void OnTimerCb(uv_timer_t* timer) {
        Timer* self = (Timer*) timer->data;
        self->on_timeout();
    }
};










class TcpSocket2 {
    using OnConnectCb = std::function<void()>;
    using OnCloseCb = std::function<void()>;
    using OnDataCb = std::function<void(std::string)>;
public:
    typedef int SocketId;

    TcpSocket2(uv_loop_t* loop_)
        : loop(loop_)
    {
        id = rand();
        on_connect = []() {};
        on_data = [](std::string) {};
        on_close_last = []() {};
    }

    TcpSocket2(SocketId id_, uv_loop_t* loop_, uv_stream_t* stream_)
        : TcpSocket2(loop_)
    {
        id = id_;
        stream = stream_;
    }

    void Write(std::string message) {
        std::cout << __FUNCTION__ << "|"
                  << name << "|" << id << " "
                  << message << std::endl;
        uv_write_t* write;
        write = (uv_write_t*) malloc(sizeof *write);
        write->data = this;

        uv_buf_t buf;
        buf.base = (char*) message.c_str();
        buf.len = message.length();

        uv_write(write, stream, &buf, 1, &TcpSocket2::WriteCb);
    }

    void Read() {
        int res;

        res = uv_read_start(
                    stream,
                    &TcpSocket2::AllocCb,
                    &TcpSocket2::ReadCb);
    }

    void Connect(std::string ipv4, int port) {
        std::cout << __FUNCTION__ << std::endl;
        closed = false;
        int res;
        struct sockaddr_in addr;
        uv_connect_t* req;

        uv_tcp_t* client = (uv_tcp_t*) malloc(sizeof uv_tcp_t);
        res = uv_tcp_init(loop, client);

        req = (uv_connect_t*) malloc(sizeof *req);
        client->data = this;
        req->data = client;
        res = uv_ip4_addr(ipv4.c_str(), port, &addr);

        stream = (uv_stream_t*) client;

        uv_tcp_nodelay(client, true);
        res = uv_tcp_connect(req, client, (const sockaddr*) &addr, &TcpSocket2::ConnectedCb);
    }

    void Close() {
        if (stream != nullptr) {
            uv_close((uv_handle_t*) stream, AfterCloseCb);
        }
    }

    void SetName(std::string name_) { name = name_; }

    const std::string& GetName() const { return name; }

    void OnConnect(OnConnectCb f) { on_connect = f; }

    void OnData(OnDataCb f) { on_data = f; }

    void OnClose(OnCloseCb f) { on_close.push_back(f); }

    // emits after all close cbs called; useful for deleting
    void OnCloseLast(OnCloseCb f) { on_close_last = f; }

    SocketId GetSocketId() { return id; }

private:
    SocketId id;
    uv_loop_t* loop;
    uv_stream_t* stream = nullptr;
    std::string name;
    bool closed = false;

    OnConnectCb on_connect;
    OnDataCb on_data;
    std::vector<OnCloseCb> on_close;
    OnCloseCb on_close_last;

    static void ConnectedCb(uv_connect_t* req, int status) {
        uv_tcp_t* client = (uv_tcp_t*) req->handle;
        TcpSocket2* socket = (TcpSocket2*) client->data;

        free(req);

        socket->on_connect();
    }

    static void WriteCb(uv_write_t* req, int status) {
        std::cout << __FUNCTION__ << status << std::endl;
//        uv_stream_t* stream = req->handle;
        free(req);
//        uv_close((uv_handle_t*) stream, AfterCloseCb);
    }

    static void AllocCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
        buf->base = (char*) malloc(suggested_size);
        buf->len = suggested_size;
    }

    static void ReadCb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
        uv_shutdown_t* sreq;
        TcpSocket2* self = (TcpSocket2*) stream->data;
        int res;

        if (nread < 0) {
            std::cout << __FUNCTION__ << nread << std::endl;
            if (nread == UV_EOF) {
                free(buf->base);
                uv_close((uv_handle_t*) stream, AfterCloseCb);
                return;
            }

            // todo: error handle
            std::cout << "error readcb: " << uv_err_name(nread) << std::endl;

            if (uv_is_writable(stream)) {
                sreq = (uv_shutdown_t*) malloc(sizeof *sreq);
                sreq->data = self;
                uv_shutdown(sreq, stream, AfterShutdownCb);
            }

            // close or stop read on error

            return;
        }

        std::stringstream ss;
        for (int i = 0; i < nread; i++) {
            char c = buf->base[i];
            ss << c;
        }

        if (nread != 0) {
            std::cout << "RECV2:" << nread << uv_is_writable(stream) << " " << ss.str() << std::endl;
        }

        self->on_data(ss.str());
    }

    static void AfterShutdownCb(uv_shutdown_t* req, int status) {
        std::cout << __FUNCTION__ << std::endl;
        uv_close((uv_handle_t*) req->handle, AfterCloseCb);
        free(req);
    }

    static void AfterCloseCb(uv_handle_t* handle) {
        TcpSocket2* self = (TcpSocket2*) handle->data;
        std::cout << __FUNCTION__ << " "
                  << self->GetName() << " "
                  << self->on_close.size() << std::endl;
        free(handle);

        for (int i = 0; i < self->on_close.size(); i++) {
            self->on_close[i]();
        }
    }
};




class TcpServer2 {
    using OnConnectionCb = std::function<void(TcpSocket2*)>;
    using OnListeningCb = std::function<void()>;
    using OnErrorCb = std::function<void()>;
    using OnCloseCb = std::function<void()>;
    using SocketId = TcpSocket2::SocketId;
    using SocketIdPool = IdPool<SocketId>;
public:
    TcpServer2(uv_loop_t* loop_)
        : loop(loop_)
    {
        on_connection = [](TcpSocket2*) {};
    }

    void Listen(const char* address, int port) {
        int res;

        res = uv_tcp_init(loop, &server);

        struct sockaddr_in server_address;
        res = uv_ip4_addr(address, port, &server_address);

        res = uv_tcp_bind(&server, (const struct sockaddr*) &server_address, 0);

        server.data = this;

        res = uv_listen((uv_stream_t*) &server, 128, &TcpServer2::OnConnected);

        is_listening = true;
    }

    bool IsListening() { return is_listening; }

    void OnConnection(OnConnectionCb f) { on_connection = f; }
private:
    uv_loop_t* loop;
    uv_tcp_t server;

    std::map<SocketId, TcpSocket2*> clients;

    SocketIdPool id_pool;

    // listeners
    OnConnectionCb on_connection;

    bool is_listening = false;

private:

    static void OnConnected(uv_stream_t* server, int status) {
        TcpServer2* self = (TcpServer2*) server->data;
        uv_stream_t* stream;
        int res;

        stream = (uv_stream_t*) malloc(sizeof(uv_tcp_t));
        res = uv_tcp_init(self->loop, (uv_tcp_t*)stream);

        res = uv_accept(server, stream);

        SocketId id = self->id_pool.Next();
        TcpSocket2* socket = new TcpSocket2(id, self->loop, stream);
        self->clients[id] = socket;

        socket->OnClose([=]() {
            self->clients.erase(socket->GetSocketId());
            self->id_pool.Free(socket->GetSocketId());
        });
        socket->OnCloseLast([socket]() {
            // or schedule delete on lib uv's next iter
            delete socket;
        });

        stream->data = socket;

        socket->Read();

        self->on_connection(socket);
    }
};






















void f(int a, int b, int c) {
    std::cout << __FUNCTION__ << a << b << c << std::endl;
}


int main(int, char**)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    using namespace google::protobuf;
    using namespace google::protobuf::io;

    Foo foo;
    foo.set_text("wicked sick");
    std::string data;
    foo.SerializeToString(&data);

    Foo foo2;
    foo2.ParseFromString(data);

    std::cout << "output" << data << data.size() << std::endl;
    std::cout << "parsed" << foo2.text() << std::endl;

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "imgui_example Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    uv_loop_t* uvloop = uv_default_loop();





//    Timer* t = new Timer(uvloop, 1000, false);
//    t->OnTimeout([t]() {
//        std::cout << "timer" << std::endl;
//    });
//    t->Start();



    struct ChatClient {
        TcpSocket2::SocketId id;
        std::string name;
        TcpSocket2* socket;
        bool show = true;
        std::string tmp_msg;

        struct Message {
            std::string text;
            bool received = false;
        };

        std::vector<Message> messages;
    };

    std::map<TcpSocket2::SocketId, ChatClient> client_chat_map;
    std::map<TcpSocket2::SocketId, ChatClient> server_chat_map;

    TcpServer2* server = new TcpServer2(uvloop);
    server->OnConnection([&](TcpSocket2* conn) {
        conn->SetName("server side");

        ChatClient cc;
        cc.socket = conn;
        cc.name = conn->GetName();
        cc.id = conn->GetSocketId();
        server_chat_map[cc.id] = cc;

        conn->OnData([conn, &server_chat_map](std::string str) {
            std::cout << "SRV: ondata:" << str << std::endl;
            ChatClient& cc2 = server_chat_map[conn->GetSocketId()];
            cc2.messages.push_back(ChatClient::Message { str });
        });

        conn->OnClose([conn, &server_chat_map]() {
            int id = conn->GetSocketId();
            std::cout << "SRV: OnClose:" << id << std::endl;

            ChatClient& cc = server_chat_map[id];
            server_chat_map.erase(id);
        });

    });
    server->Listen("0.0.0.0", 3001);

    std::set<TcpSocket2*> sockets;



    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }



        {
            uv_run(uvloop, UV_RUN_NOWAIT);
        }


        {
            ImGui::Begin("Tcp Server");

            static int srvport = 3001;
            ImGui::InputInt("srvport", &srvport);

            if (server->IsListening()) {
                ImGui::LabelText("", "server is listening");
            }
            else {
                if (ImGui::Button("Start server")) {
                    server->Listen("0.0.0.0", 3001);
                }
            }




            ImGui::LabelText("", "Message");
            static std::string msg;
            ImGui::InputText("server msg", &msg);


            for (auto& [key, val] : server_chat_map) {
                TcpSocket2::SocketId id = key;
                ChatClient& cc = val;
                ImGui::PushID(cc.id);
                ImGui::LabelText("client", "socket:%d", cc.id); ImGui::SameLine();
                if (ImGui::Button("Send")) {
                    std::cout << "server sending to" << cc.socket->GetName() << std::endl;
                    cc.socket->Write(msg);
                    msg = "";
                }
                ImGui::PopID();
            }


//            for (auto& v : server_clients) {
//                ImGui::LabelText("client", "socket:%d", v); ImGui::SameLine();
//                if (ImGui::Button("Send")) {
//                    TcpSocket2* conn = server_clients_map[v];
//                    conn->Write(msg);
//                }
//            }

            ImGui::End();
        }

        {
            ImGui::Begin("Open Connection");

            if (ImGui::Button("Open connection")) {
                ChatClient client;
                TcpSocket2* socket = new TcpSocket2(uvloop);

                socket->OnConnect([]() {
                    std::cout << "client onconnect" << std::endl;
                });

                socket->Connect("0.0.0.0", 3001);

                client.id = socket->GetSocketId();
                client.socket = socket;
                client.name = string_format("name %d", socket->GetSocketId());

                client_chat_map[client.id] = client;

                socket->OnClose([socket, &client_chat_map]() {
                    client_chat_map.erase(socket->GetSocketId());
                });

                socket->OnData([socket, &client_chat_map](std::string data) {
                    std::cout << "client on data" << std::endl;
                    ChatClient& cc = client_chat_map[socket->GetSocketId()];
                    cc.messages.push_back(ChatClient::Message { data });
                });
            }

            ImGui::End();
        }




        {


            std::vector<TcpSocket2::SocketId> remove;
            for (auto& [key, val] : client_chat_map) {
                TcpSocket2::SocketId id = key;
                ChatClient& cc = val;
                cc.socket->Read();

                bool exshow = cc.show;
                if (cc.show) {
                    if (!ImGui::Begin(cc.name.c_str(), &cc.show)) {
                        ImGui::End();
                    }
                    else {
                        ImGui::LabelText("msgs", "num of msgs %d", cc.messages.size());

                        for (auto& msg : cc.messages) {
                            ImGui::TextWrapped("%s", msg.text.c_str());
                            ImGui::SetScrollY(ImGui::GetScrollMaxY());
                        }

                        ImGui::InputText("", &cc.tmp_msg); ImGui::SameLine();
                        if (ImGui::Button("Send")) {
                            cc.socket->Write(cc.tmp_msg);
                            cc.tmp_msg = "";
                        }

                        ImGui::End();
                    }

                    if (exshow && !cc.show) {
                        remove.push_back(id);
                        cc.socket->Close();
                    }
                }
            }

            for (TcpSocket2::SocketId id : remove) {
                client_chat_map.erase(id);
            }

        }






















        {


//            std::vector<TcpSocket2::SocketId> remove;
//            std::cout << "chats " << chat_clients.size() << std::endl;
//            for (ChatClient& cc : chat_clients) {
//                bool exshow = cc.show;
//                if (cc.show) {
//                    if (!ImGui::Begin(cc.name.c_str(), &cc.show)) {
//                        ImGui::End();
//                    }
//                    else {


////                    std::string text = string_format("client:%d", cc.socket->GetSocketId());
////                    ImGui::LabelText("", text.c_str());

//                        ImGui::End();
//                    }

//                    if (exshow && !cc.show) {
//                        remove.push_back(cc.socket->GetSocketId());
//                        cc.socket->Close();
//                    }
//                }
//            }

//            // remove closed connections
//            for (auto id : remove) {
//                chat_clients.erase(
//                            std::remove_if(chat_clients.begin(),
//                                           chat_clients.end(),
//                                           [id](ChatClient& cc) { return cc.id == id; }),
//                        chat_clients.end());
//                server_clients.erase(id);
//            }









            {
//                ImGui::Begin("Client");

//                for (auto it = sockets.begin(); it != sockets.end(); ++it) {
//                    TcpSocket2* socket = *it;
//                    ImGui::LabelText("socket", "socket:%d", socket->GetSocketId());
//                    ImGui::Button("Open window");
//                }

//                ImGui::End();
            }
        }


//        {
//            bool bres = ImGui::Begin("Tcp client");
//            std::cout << bres << std::endl;


//            static std::string ipv4 = "0.0.0.0";
//            ImGui::InputText("srvaddr", &ipv4);
//            static int port = 3001;
//            ImGui::InputInt("srvport", &port);
//            static std::string msg = "";
//            ImGui::InputText("msg", &msg);

//            if (ImGui::Button("send local")) {
////                TcpConnection* conn = new TcpConnection(uvloop);
////                conn->SetMessage(msg);

//                TcpSocket2* socket = new TcpSocket2(uvloop);
//                socket->SetName("client side");
//                socket->Connect("0.0.0.0", 3001);
//                socket->OnClose([socket]() {
//                    std::cout << socket->GetName() << " closed" << std::endl;
//                });
//                socket->OnConnect([socket]() {
//                    socket->Write("message from client to server");
//                });
//            }

//            ImGui::End();
//        }












        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
