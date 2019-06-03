module Uuid_T = {
  include Uuidm;

  let compare = Uuidm.compare;

  let sexp_of_t = (t): Base.Sexp.t => Base.Sexp.Atom(Uuidm.to_string(t));
};


include Uuid_T;
include Base.Comparator.Make(Uuid_T);

let randomState = Random.State.make([|1, 2, 3|]);

let generateId = () => v4_gen(randomState, ());
