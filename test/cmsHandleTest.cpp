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
  iEvent.getByToken(token, h);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced with getHandle(EDGetTokenT<Foo>&) as shown. [cms-handle]
// CHECK-FIXES: {{^}}  h = iEvent.getHandle(token);{{$}}
}

void doWork2( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  using namespace edm;
  Handle<Foo> h;
  iEvent.getByToken(token, h);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced with getHandle(EDGetTokenT<Foo>&) as shown. [cms-handle]
// CHECK-FIXES: {{^}}  h = iEvent.getHandle(token);{{$}}
}

void doWork3( edm::Event& iEvent, edm::EDGetTokenT<Foo> const * ptoken) {
  using namespace edm;
  edm::Handle<Foo> h;
 iEvent.getByToken(*ptoken, h);
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced with getHandle(EDGetTokenT<Foo>&) as shown. [cms-handle]
// CHECK-FIXES: {{^}} h = iEvent.getHandle(*ptoken);{{$}}
  Foo const& f = *h;
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: const& T var = *edm::Handle<T> type variable and variable is initialized by edm::Event type variable .getByToken(edm::EDGetTokenT<T>; edm::Handle<T>) can be replaced with const& T var = edm::Event type variable .get(edm::EDGetToken<T>); [cms-handle]
// CHECK-FIXES: {{^}}  Foo const& f = iEvent.get(edm::EDGetTokenT<T>);{{$}}
}
