namespace edm {

  template<typename T>
  class EDGetTokenT {
  };

  template<typename T>
  class Handle {
  public:
    Handle() {};
    T const& operator*() {};
    T const& get(edm::EDGetTokenT<T>) {};
  };

  class Event {

  public:
    template<typename T>
    void getByToken( edm::EDGetTokenT<T>, Handle<T>&);
    template<typename T>
    Handle<T> getHandle( edm::EDGetTokenT<T> );
  };
}

struct Foo {};

void doWork( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  edm::Handle<Foo> h;
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: use function iEvent.getHandle(token) to initialize edm::Handle<Foo> [cms-handle]
// CHECK-FIXES: {{^}}  //commented out by CMS clang-tidy getHandle rewriter edm::Handle<Foo> h;{{$}}
  iEvent.getByToken(token, h);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced with getHandle(EDGetTokenT<Foo>&) as shown. [cms-handle]
// CHECK-FIXES: {{^}}  auto h = iEvent.getHandle(token);{{$}}
  Foo const& f = *h;

}

void doWork2( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  using namespace edm;
  Handle<Foo> h;
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: use function iEvent.getHandle(token) to initialize edm::Handle<Foo> [cms-handle]
// CHECK-FIXES: {{^}}  //commented out by CMS clang-tidy getHandle rewriter Handle<Foo> h;{{$}}
  iEvent.getByToken(token, h);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced with getHandle(EDGetTokenT<Foo>&) as shown. [cms-handle]
// CHECK-FIXES: {{^}}  auto h = iEvent.getHandle(token);{{$}}
  Foo const& f = *h;
}

void doWork3( edm::Event& iEvent, edm::EDGetTokenT<Foo> const * ptoken) {
  edm::Handle<Foo> h;
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: use function iEvent.getHandle(*ptoken) to initialize edm::Handle<Foo> [cms-handle]
// CHECK-FIXES: {{^}}  //commented out by CMS clang-tidy getHandle rewriter edm::Handle<Foo> h;{{$}}
  iEvent.getByToken(*ptoken, h);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced wit
// CHECK-FIXES: {{^}}  auto h = iEvent.getHandle(*ptoken);{{$}}
  // no match because token is not pass in directly by ref.
}
