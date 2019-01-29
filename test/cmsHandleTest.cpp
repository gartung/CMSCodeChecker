namespace edm {

  template<typename T>
  class EDGetTokenT {
  };

  template<typename T>
  class Handle {
  public:
    Handle() {};
    T const& operator*() {};
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
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(getByToken<Foo>&, Handle<>&) is deprecated and should be replaced with getHandle(EDGetTokenT<>&) as shown. [cms-handle]
// CHECK-FIXES: {{^}}  h = iEvent.getHandle(token);{{$}}
}

void doWork2( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  using namespace edm;
  Handle<Foo> h;
  iEvent.getByToken(token, h);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(getByToken<Foo>&, Handle<>&) is deprecated and should be replaced with getHandle(EDGetTokenT<>&) as shown. [cms-handle]
// CHECK-FIXES: {{^}}  h = iEvent.getHandle(token);{{$}}
  Foo const& f = *h;
}

void doWork3( edm::Event& iEvent, edm::EDGetTokenT<Foo> const * ptoken) {
  using namespace edm;
  Handle<Foo> h;
 iEvent.getByToken(*ptoken, h);
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: function getByToken(getByToken<Foo>&, Handle<>&) is deprecated and should be replaced with getHandle(EDGetTokenT<>&) as shown. [cms-handle]
// CHECK-FIXES: {{^}} h = iEvent.getHandle(*ptoken);{{$}}
  Foo const& f = *h;
}
