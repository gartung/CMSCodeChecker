namespace edm {

  template<typename T>
  class EDGetTokenT {
  };

  template<typename T>
  class Handle {
  public:
    Handle() {};
    Handle(T const* m): m_t(m) {};
    T * m_t;
    T const& operator*() {return *m_t;};
    T const* operator->() const {return m_t;};
    T const& get(edm::EDGetTokenT<T>) {};
    explicit operator bool () const {return not( m_t == nullptr);};
    bool operator!() const { return m_t == nullptr;};
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
  bool doWork( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token);
  edm::EDGetTokenT<Foo> m_token;
  edm::Handle<Foo> m_handle;
};

bool Bar::doWork( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  edm::Handle<Foo> handleVar;
  iEvent.getByToken(token, handleVar);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: direct call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with handleVar = iEvent.getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}  handleVar = iEvent.getHandle(token);{{$}}
  Foo const& f = *handleVar;
  edm::Handle<Foo> handleVarM;
  iEvent.getByToken(m_token, handleVarM);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: direct call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with handleVarM = iEvent.getHandle(m_token). [cms-handle]
// CHECK-FIXES: {{^}}  handleVarM = iEvent.getHandle(m_token);{{$}}
  bool hasHandleM = iEvent.getByToken(m_token,m_handle);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: bool return call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with bool(m_handle = iEvent.getHandle(m_token)). [cms-handle]
// CHECK-FIXES: {{^}}  bool hasHandleM = bool(m_handle = iEvent.getHandle(m_token));{{$}}
  hasHandleM = iEvent.getByToken(m_token,m_handle);
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: bool return call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with bool(m_handle = iEvent.getHandle(m_token)). [cms-handle]
// CHECK-FIXES: {{^}}  hasHandleM = bool(m_handle = iEvent.getHandle(m_token));{{$}}
 
  using namespace edm;
  Handle<Foo> handleVar2;  
  Handle<Foo> handleVar21;  

  if (iEvent.getByToken(token, handleVar2)) { Foo const& f = *handleVar;}
// CHECK-MESSAGES: :[[@LINE-1]]:7: warning: bool return call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with (handleVar2 = iEvent.getHandle(token)). [cms-handle]
// CHECK-FIXES: {{^}}  if ((handleVar2 = iEvent.getHandle(token))) { Foo const& f = *handleVar;}{{$}}
  if (!iEvent.getByToken(token, handleVar2)) { Foo const& f = *handleVar;}
// CHECK-MESSAGES: :[[@LINE-1]]:8: warning: bool return call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with (handleVar2 = iEvent.getHandle(token)). [cms-handle]
// CHECK-FIXES: {{^}}  if (!(handleVar2 = iEvent.getHandle(token))) { Foo const& f = *handleVar;}{{$}}
  try { iEvent.getByToken(token, handleVar21); }
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: direct call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with handleVar21 = iEvent.getHandle(token). [cms-handle]

// CHECK-FIXES: {{^}}  try { handleVar21 = iEvent.getHandle(token); }{{$}}



  catch (bool) {}

  int i = 0;
  edm::Handle<Foo> handleVar3;
  if (i == 0) { iEvent.getByToken(token, handleVar3); }
// CHECK-MESSAGES: :[[@LINE-1]]:17: warning: direct call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with handleVar3 = iEvent.getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}  if (i == 0) { handleVar3 = iEvent.getHandle(token); }{{$}}
  else if (i == 1) { edm::Handle<Foo> handleVarT;
               iEvent.getByToken(token, handleVarT); }
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: direct call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with handleVarT = iEvent.getHandle(token). [cms-handle]
// CHECK-FIXES: {{^}}               handleVarT = iEvent.getHandle(token); }{{$}}
  

  edm::EDGetTokenT<Foo> const * ptoken(&token);
  edm::Handle<Foo> handleVar4;
  iEvent.getByToken(*ptoken, handleVar4);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: direct call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with handleVar4 = iEvent.getHandle(*ptoken). [cms-handle]
// CHECK-FIXES: {{^}}  handleVar4 = iEvent.getHandle(*ptoken);{{$}}

  edm::Handle<Foo> handleVar5;
  bool hasHandle = iEvent.getByToken(token, handleVar5);
// CHECK-MESSAGES: :[[@LINE-1]]:20: warning: bool return call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with bool(handleVar5 = iEvent.getHandle(token)). [cms-handle]
// CHECK-FIXES: {{^}}  bool hasHandle = bool(handleVar5 = iEvent.getHandle(token));{{$}}
  bool hasHandleP = iEvent.getByToken(*ptoken, handleVar5);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: bool return call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with bool(handleVar5 = iEvent.getHandle(*ptoken)). [cms-handle]
// CHECK-FIXES: {{^}}  bool hasHandleP = bool(handleVar5 = iEvent.getHandle(*ptoken));{{$}}
  return iEvent.getByToken(token, handleVar5);
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: bool return call of function getByToken(EDGetTokenT<>&, Handle<>&) is deprecated and should be replaced here with bool(handleVar5 = iEvent.getHandle(token)). [cms-handle]
// CHECK-FIXES: {{^}}  return bool(handleVar5 = iEvent.getHandle(token));{{$}}
}
