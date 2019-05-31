include Base;

module Uuid_T = {
  include Uuidm;

  let compare = Uuidm.compare;

  let sexp_of_t = (t): Sexp.t => Sexp.Atom(Uuidm.to_string(t));
};

include Uuid_T;
include Comparator.Make(Uuid_T);
