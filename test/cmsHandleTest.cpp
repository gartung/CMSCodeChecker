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
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: use function iEvent.getHandle(token) to initialize edm::Handle<Foo> h [cms-handle]
// CHECK-FIXES: {{^}}  edm::Handle<Foo> h = iEvent.getHandle(token);{{$}}
  iEvent.getByToken(token, h);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(token, h) is deprecated and should be removed and replaced with getHandle(token) as shown above. [cms-handle]
// CHECK-FIXES: {{^}}  //;{{$}}
  Foo const& f = *h;
}
