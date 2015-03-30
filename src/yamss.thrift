namespace cpp yamss.server

exception YamssException {
  1: string what
}

service Yamss {

  void run(1: string a_url) throws (1: YamssException e),

}
