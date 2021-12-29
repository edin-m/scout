#include <iostream>

#include <map>
#include <unordered_map>
#include <string>
#include <vector>

class Class {
public:
    Class() {
        std::cout << __FUNCTION__ << " " << name << std::endl;
    }
    Class(const Class& c) {
        name = c.name + " copy";
        std::cout << __FUNCTION__ << " " << name << std::endl;
    }
    Class(const Class&& c) {
        name = c.name + " move";
        std::cout << __FUNCTION__ << " " << name << std::endl;
    }
    ~Class() {
        std::cout << __FUNCTION__ << " " << name << std::endl;
    }
    Class& operator=(const Class& c) {
        name = c.name + " op";
        std::cout << __FUNCTION__ << " " << name << std::endl;
        return *this;
    }
    Class& operator=(const Class&& c) {
        name = c.name + " move op";
        std::cout << __FUNCTION__ << " " << name << std::endl;
        return *this;
    }

    std::string name;
};

Class f2(Class c) {
    Class c1(c);
    return c1;
}

Class f3(Class& c) {
    Class c1(c);
    return c1;
}

Class f4(Class& c) {
    return c;
}

Class* f5(Class& c) {
    return new Class(c);
}


int main(int argc, char** argv) {
    std::cout << "Hello World" << std::endl;

    std::cout << "======= 1 =======" << std::endl;
    Class c1; c1.name = "c1";

    {
        /**
            Class::Class
            Class::Class c1 copy
            Class::~Class c1 copy
         */
        Class c2(c1);
    }

    std::cout << "======= 2 =======" << std::endl;
    {
        /**
            Class::Class c1 copy
            Class::Class c1 copy copy
            Class::Class c1 copy copy move
            Class::~Class c1 copy copy
            Class::~Class c1 copy
            Class::~Class c1 copy copy move
         */
        Class c3 = f2(c1);
    }

    std::cout << "======= 3 =======" << std::endl;
    {
        /**
            Class::Class c1 copy
            Class::Class c1 copy move
            Class::~Class c1 copy
            Class::~Class c1 copy move
         */
        Class c3 = f3(c1);
    }

    std::cout << "======= 4 =======" << std::endl;
    {
        /**
            Class::Class
            Class::Class c4 copy
            Class::~Class c4 copy
            Class::~Class c4
         */
        Class c4; c4.name = "c4";
        Class c3 = f4(c4);
    }

    std::cout << "======= 5 =======" << std::endl;
    {
        /**
            Class::Class
            Class::Class  copy
            Class::~Class
            Class::Class  copy copy
            Class::~Class  copy
            Class::~Class  copy copy
         */
        std::vector<Class> vec;
        Class c2 = f4(Class());
        vec.push_back(c2);
    }

    std::cout << "======= 6 =======" << std::endl;
    {
        /**
            Class::Class
            Class::Class  copy
            Class::Class  copy move
            Class::~Class  copy
            Class::~Class
            Class::~Class  copy move
         */
        std::vector<Class> vec;
        vec.push_back(f4(Class()));
    }

    std::cout << "======= 7 =======" << std::endl;
    {
        /**
            Class::Class
            Class::operator = c1 op
            Class::~Class c1 op
         */
        Class c2;
        c2 = c1;
    }

    std::cout << "======= 8 =======" << std::endl;
    {
        /**
            Class::Class c1 copy
            Class::~Class c1 copy
         */
        Class* c2 = f5(c1);
        Class c3 = *c2;
        delete c2;
    }

    std::cout << "======= end =======" << std::endl;

    return 0;
}
