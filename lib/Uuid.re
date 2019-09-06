open! Base;

/* code taken from https://github.com/dbuenzli/uuidm and adapted for use with Base */

type t = string;
let rand = (s, ()) => Random.State.bits(s); /* 30 random bits generator. */
let v4_ocaml_random_uuid = rand => {
  let r0 = rand();
  let r1 = rand();
  let r2 = rand();
  let r3 = rand();
  let r4 = rand();
  let u = Bytes.create(16);
  Bytes.set(u, 0) @@ Char.of_int_exn(r0 land 255);
  Bytes.set(u, 1) @@ Char.of_int_exn(r0 lsr 8 land 255);
  Bytes.set(u, 2) @@ Char.of_int_exn(r0 lsr 16 land 255);
  Bytes.set(u, 3) @@ Char.of_int_exn(r1 land 255);
  Bytes.set(u, 4) @@ Char.of_int_exn(r1 lsr 8 land 255);
  Bytes.set(u, 5) @@ Char.of_int_exn(r1 lsr 16 land 255);
  Bytes.set(u, 6) @@ Char.of_int_exn(64 lor (r1 lsr 24 land 15));
  Bytes.set(u, 7) @@ Char.of_int_exn(r2 land 255);
  Bytes.set(u, 8) @@ Char.of_int_exn(128 lor (r2 lsr 24 land 63));
  Bytes.set(u, 9) @@ Char.of_int_exn(r2 lsr 8 land 255);
  Bytes.set(u, 10) @@ Char.of_int_exn(r2 lsr 16 land 255);
  Bytes.set(u, 11) @@ Char.of_int_exn(r3 land 255);
  Bytes.set(u, 12) @@ Char.of_int_exn(r3 lsr 8 land 255);
  Bytes.set(u, 13) @@ Char.of_int_exn(r3 lsr 16 land 255);
  Bytes.set(u, 14) @@ Char.of_int_exn(r4 land 255);
  Bytes.set(u, 15) @@ Char.of_int_exn(r4 lsr 8 land 255);
  Bytes.unsafe_to_string(~no_mutation_while_string_reachable=u);
};

let to_string = (~upper=false, u) => {
  let hbase = if (upper) {55} else {87};
  let hex = (hbase, i) =>
    Char.of_int_exn(
      if (i < 10) {
        48 + i;
      } else {
        hbase + i;
      },
    );
  let s = Bytes.of_string("XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX");
  let i = ref(0);
  let j = ref(0);
  let byte = (s, i, c) => {
    Bytes.set(s, i) @@ hex(hbase, c lsr 4);
    Bytes.set(s, i + 1) @@ hex(hbase, c land 15);
  };

  while (j^ < 4) {
    byte(s, i^, Char.to_int(u.[j^]));
    i := i^ + 2;
    Int.incr(j);
  };
  Int.incr(i);
  while (j^ < 6) {
    byte(s, i^, Char.to_int(u.[j^]));
    i := i^ + 2;
    Int.incr(j);
  };
  Int.incr(i);
  while (j^ < 8) {
    byte(s, i^, Char.to_int(u.[j^]));
    i := i^ + 2;
    Int.incr(j);
  };
  Int.incr(i);
  while (j^ < 10) {
    byte(s, i^, Char.to_int(u.[j^]));
    i := i^ + 2;
    Int.incr(j);
  };
  Int.incr(i);
  while (j^ < 16) {
    byte(s, i^, Char.to_int(u.[j^]));
    i := i^ + 2;
    Int.incr(j);
  };
  Bytes.unsafe_to_string(~no_mutation_while_string_reachable=s);
};

let pp = (ppf, u) => Caml.Format.pp_print_string(ppf, to_string(u));

let v4_gen = seed => {
  let rand = rand(seed);
  fun
  | () => v4_ocaml_random_uuid(rand);
};

let generateId = () => {
  let randomState = Random.State.default;
  v4_gen(randomState, ());
};
