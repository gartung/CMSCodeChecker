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
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: use function iEvent.getHandle(token) to initialize variable h [cms-handle]
// CHECK-FIXES: {{^}}  edm::Handle<Foo> h = iEvent.getHandle(token);{{$}}
  iEvent.getByToken(token, h);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be removed here and replaced with getHandle(token) to inialize variable h. [cms-handle]
  Foo const& f = *h;

}

void doWork2( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  using namespace edm;
  Handle<Foo> h;
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: use function iEvent.getHandle(token) to initialize variable h [cms-handle]
// CHECK-FIXES: {{^}}  Handle<Foo> h = iEvent.getHandle(token);{{$}}
  iEvent.getByToken(token, h);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be removed here and replaced with getHandle(token) to inialize variable h. [cms-handle]
  Foo const& f = *h;
}

void doWork3( edm::Event& iEvent, edm::EDGetTokenT<Foo> const * ptoken) {
  edm::Handle<Foo> h;
  iEvent.getByToken(*ptoken, h);
  // no match because token is not pass in directly by ref.
}
