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

class Bar {
  public:
  void doWork( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token);
  edm::EDGetTokenT<Foo> m_token;
};

void Bar::doWork( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  edm::Handle<Foo> handleVar;
// CHECK-MESSAGES: :[[@LINE-1]]:29: warning: use function iEvent.getHandle(token) to initialize variable handleVar [cms-handle]
// CHECK-FIXES: {{^}}  edm::Handle<Foo> handleVar = iEvent.getHandle(token);{{$}}
  iEvent.getByToken(token, handleVar);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be removed here and replaced with getHandle(token) to inialize variable handleVar. [cms-handle]
  Foo const& f = *handleVar;
  edm::Handle<Foo> handleVarM;
// CHECK-MESSAGES: :[[@LINE-1]]:30: warning: use function iEvent.getHandle(m_token) to initialize variable handleVarM [cms-handle]
// CHECK-FIXES: {{^}}  edm::Handle<Foo> handleVarM = iEvent.getHandle(m_token);{{$}}
  iEvent.getByToken(m_token, handleVarM);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be removed here and replaced with getHandle(m_token) to inialize variable handleVarM. [cms-handle]

  using namespace edm;
  Handle<Foo> handleVar2;  
  Handle<Foo> handleVar21;  

  if (iEvent.getByToken(token, handleVar2)) { Foo const& f = *handleVar;}
// CHECK-MESSAGES: :[[@LINE-1]]:7: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token) to inialize variable handleVar2. [cms-handle]
// CHECK-FIXES: {{^}}  if ((handleVar2 = iEvent.getHandle(token))) { Foo const& f = *handleVar;}{{$}}
  if (!iEvent.getByToken(token, handleVar2)) { Foo const& f = *handleVar;}
// CHECK-MESSAGES: :[[@LINE-1]]:8: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token) to inialize variable handleVar2. [cms-handle]
// CHECK-FIXES: {{^}}  if (!(handleVar2 = iEvent.getHandle(token))) { Foo const& f = *handleVar;}{{$}}
  try { iEvent.getByToken(token, handleVar21); }
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token) to inialize variable handleVar21. [cms-handle]
// CHECK-FIXES: {{^}}  try { handleVar21 = iEvent.getHandle(token); }{{$}}



  catch (bool) {}

  int i = 0;
  edm::Handle<Foo> handleVar3;
  if (i = 0) { iEvent.getByToken(token, handleVar3); }
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token) to inialize variable handleVar3. [cms-handle]
// CHECK-FIXES: {{^}}  if (i = 0) { handleVar3 = iEvent.getHandle(token); }{{$}}
  else if (i = 1) { edm::Handle<Foo> handleVarT;
               iEvent.getByToken(token, handleVarT); }
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token) to inialize variable handleVarT. [cms-handle]
// CHECK-FIXES: {{^}}               handleVarT = iEvent.getHandle(token); }{{$}}

  

  edm::EDGetTokenT<Foo> const * ptoken(&token);
  edm::Handle<Foo> handleVar4;
// CHECK-MESSAGES: :[[@LINE-1]]:30: warning: use function iEvent.getHandle(*ptoken) to initialize variable handleVar4 [cms-handle]
// CHECK-FIXES: {{^}}  edm::Handle<Foo> handleVar4 = iEvent.getHandle(*ptoken);{{$}}
  iEvent.getByToken(*ptoken, handleVar4);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be removed here and replaced with getHandle(*ptoken) to inialize variable handleVar4.  [cms-handle]

}
