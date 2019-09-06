type t;

let generateId: unit => t;
let to_string: (~upper: bool=?, t) => string;
let pp: (Formatter.t, t) => unit;
