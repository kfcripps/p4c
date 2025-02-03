extern void __e(in bit<28> arg);

control C() {
    action foo(in bit<8> x) {
        switch (x) {
            0 :       { __e(2); }
            default : { __e(3); }
        }
    }

    table t {
        actions = { foo(1); }
        default_action = foo(1);
    }

    apply {
        t.apply();
    }
}

control proto();
package top(proto p);

top(C()) main;
