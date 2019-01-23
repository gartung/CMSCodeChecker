namespace edm {

  template<typename T>
  class EDGetTokenT {
  };

  template<typename T>
  class Handle {
  public:
    Handle() {};
  };

  class Event {

  public:
    template<typename T>
    void getByToken( edm::EDGetTokenT<T>, Handle<T>&);
  };
}

struct Foo {};

void doWork( edm::Event& iEvent, edm::EDGetTokenT<Foo> const& token) {
  edm::Handle<Foo> h;
  iEvent.getByToken(token, h);
}
