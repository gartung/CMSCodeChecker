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
  edm::Handle<Foo> m_handle;
};

void Bar::doWork( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  edm::Handle<Foo> handleVar;
  iEvent.getByToken(token, handleVar);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: direct call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}  handleVar = iEvent.getHandle(token);{{$}}
  Foo const& f = *handleVar;
  edm::Handle<Foo> handleVarM;
  iEvent.getByToken(m_token, handleVarM);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: direct call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(m_token). [cms-handle]
// CHECK-FIXES: {{^}}  handleVarM = iEvent.getHandle(m_token);{{$}}

  using namespace edm;
  Handle<Foo> handleVar2;  
  Handle<Foo> handleVar21;  

  if (iEvent.getByToken(token, handleVar2)) { Foo const& f = *handleVar;}
// CHECK-MESSAGES: :[[@LINE-1]]:7: warning: bool return call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}  if ((handleVar2 = iEvent.getHandle(token))) { Foo const& f = *handleVar;}{{$}}
  if (!iEvent.getByToken(token, handleVar2)) { Foo const& f = *handleVar;}
// CHECK-MESSAGES: :[[@LINE-1]]:8: warning: bool return call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}  if (!(handleVar2 = iEvent.getHandle(token))) { Foo const& f = *handleVar;}{{$}}
  try { iEvent.getByToken(token, handleVar21); }
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: direct call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}  try { handleVar21 = iEvent.getHandle(token); }{{$}}



  catch (bool) {}

  int i = 0;
  edm::Handle<Foo> handleVar3;
  if (i = 0) { iEvent.getByToken(token, handleVar3); }
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: direct call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}  if (i = 0) { handleVar3 = iEvent.getHandle(token); }{{$}}
  else if (i = 1) { edm::Handle<Foo> handleVarT;
               iEvent.getByToken(token, handleVarT); }
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: direct call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}               handleVarT = iEvent.getHandle(token); }{{$}}
  

  edm::EDGetTokenT<Foo> const * ptoken(&token);
  edm::Handle<Foo> handleVar4;
  iEvent.getByToken(*ptoken, handleVar4);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: direct call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(*ptoken). [cms-handle]
// CHECK-FIXES: {{^}}  handleVar4 = iEvent.getHandle(*ptoken);{{$}}

  edm::Handle<Foo> handleVar5;
  bool hasHandle = iEvent.getByToken(token, handleVar5);
// CHECK-MESSAGES: :[[@LINE-1]]:20: warning: bool return call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}  bool hasHandle = (handleVar5 = iEvent.getHandle(token));{{$}}
  bool hasHandleM = iEvent.getByToken(m_token,m_handle);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: bool return call of function getByToken(EDGetTokenT<Foo>&, Handle<Foo>&) is deprecated and should be replaced here with getHandle(m_token). [cms-handle]
// CHECK-FIXES: {{^}}  bool hasHandleM = (m_handle = iEvent.getHandle(m_token));{{$}}
}
