extern void foo();
extern void bar();
extern bit<8> baz();
action a(){
    switch(baz()) {
        1 + 2 == 3 ? 1 : 2 : { foo(); }
        3 + 4 == 0 ? 3 : 4 : { bar(); }
    }
}
action b(){}
action NoAction(){}
control c() {
    table t  {
        actions = { a ; b;  }
    }
    apply {
        t.apply();
    }
}

control C();
package top(C c);

top(c()) main;
