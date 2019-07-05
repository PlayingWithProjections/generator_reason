open! Base;

type t;
type comparator_witness;

let comparator: Comparator.t(t, comparator_witness);

let generateId: unit => t;
let to_string: (~upper: bool=?, t) => string;
let pp: (Formatter.t, t) => unit;
let hash: t => int;
let compare: (t, t) => int;
let sexp_of_t: t => Base.Sexp.t;
