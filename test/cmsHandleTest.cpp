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
    bool getByToken( edm::EDGetTokenT<T>, Handle<T>&);
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

  using namespace edm;
  Handle<Foo> handleVar2;  

  if (iEvent.getByToken(token, handleVar2)) { Foo const& f = *handleVar;}
// CHECK-MESSAGES: :[[@LINE-1]]:7: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token) to inialize variable handleVar2. [cms-handle]
// CHECK-FIXES: {{^}}  if (handleVar2 = iEvent.getHandle(token) { Foo const& f = *handleVar;}{{$}}
  try { iEvent.getByToken(token, handleVar2); }
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token) to inialize variable handleVar2. [cms-handle]
// CHECK-FIXES: {{^}}  try { handleVar2 = iEvent.getHandle(token) }{{$}}



  catch (bool) {}

  int i = 0;
  edm::Handle<Foo> handleVar3;
  if (i = 0) { iEvent.getByToken(token, handleVar3); }
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token) to inialize variable handleVar3. [cms-handle]
// CHECK-FIXES: {{^}}  if (i = 0) { handleVar3 = iEvent.getHandle(token) }{{$}}

  

  edm::EDGetTokenT<Foo> const * ptoken(&token);
  edm::Handle<Foo> handleVar4;
// CHECK-MESSAGES: :[[@LINE-1]]:30: warning: use function iEvent.getHandle(*ptoken) to initialize variable handleVar4 [cms-handle]
// CHECK-FIXES: {{^}}  edm::Handle<Foo> handleVar4 = iEvent.getHandle(*ptoken);{{$}}
  iEvent.getByToken(*ptoken, handleVar4);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be removed here and replaced with getHandle(*ptoken) to inialize variable handleVar4. [cms-handle]
}
