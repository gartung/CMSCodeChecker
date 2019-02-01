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
  edm::Handle<Foo> handleVar;
// CHECK-MESSAGES: :[[@LINE-1]]:29: warning: use function iEvent.getHandle(token) to initialize variable handleVar [cms-handle]
// CHECK-FIXES: {{^}}  edm::Handle<Foo> handleVar = iEvent.getHandle(token);{{$}}
  iEvent.getByToken(token, handleVar);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be removed here and replaced with getHandle(token) to inialize variable handleVar. [cms-handle]
  Foo const& f = *handleVar;

}

void doWork2( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  using namespace edm;
  Handle<Foo> handleVar;
// CHECK-MESSAGES: :[[@LINE-1]]:24: warning: use function iEvent.getHandle(token) to initialize variable handleVar [cms-handle]
// CHECK-FIXES: {{^}}  Handle<Foo> handleVar = iEvent.getHandle(token);{{$}}
  iEvent.getByToken(token, handleVar);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be removed here and replaced with getHandle(token) to inialize variable handleVar. [cms-handle]
  Foo const& f = *handleVar;
}

void doWork3( edm::Event& iEvent, edm::EDGetTokenT<Foo> const * ptoken) {
  edm::Handle<Foo> handleVar;
// CHECK-MESSAGES: :[[@LINE-1]]:29: warning: use function iEvent.getHandle(*ptoken) to initialize variable handleVar [cms-handle]
// CHECK-FIXES: {{^}}  edm::Handle<Foo> handleVar = iEvent.getHandle(*ptoken);{{$}}
  iEvent.getByToken(*ptoken, handleVar);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be removed here and replaced with getHandle(*ptoken) to inialize variable handleVar. [cms-handle]
  // no match because token is not pass in directly by ref.
}
