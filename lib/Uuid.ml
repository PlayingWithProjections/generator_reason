type t = string

let rand s () = Random.State.bits s

let v4_ocaml_random_uuid rand =
  let r0 = rand () in
  let r1 = rand () in
  let r2 = rand () in
  let r3 = rand () in
  let r4 = rand () in
  let u = Bytes.create 16 in
  Bytes.set u 0 @@ Char.of_int_exn (r0 land 255);
  Bytes.set u 1 @@ Char.of_int_exn ((r0 lsr 8) land 255);
  Bytes.set u 2 @@ Char.of_int_exn ((r0 lsr 16) land 255);
  Bytes.set u 3 @@ Char.of_int_exn (r1 land 255);
  Bytes.set u 4 @@ Char.of_int_exn ((r1 lsr 8) land 255);
  Bytes.set u 5 @@ Char.of_int_exn ((r1 lsr 16) land 255);
  Bytes.set u 6 @@ Char.of_int_exn (64 lor ((r1 lsr 24) land 15));
  Bytes.set u 7 @@ Char.of_int_exn (r2 land 255);
  Bytes.set u 8 @@ Char.of_int_exn (128 lor ((r2 lsr 24) land 63));
  Bytes.set u 9 @@ Char.of_int_exn ((r2 lsr 8) land 255);
  Bytes.set u 10 @@ Char.of_int_exn ((r2 lsr 16) land 255);
  Bytes.set u 11 @@ Char.of_int_exn (r3 land 255);
  Bytes.set u 12 @@ Char.of_int_exn ((r3 lsr 8) land 255);
  Bytes.set u 13 @@ Char.of_int_exn ((r3 lsr 16) land 255);
  Bytes.set u 14 @@ Char.of_int_exn (r4 land 255);
  Bytes.set u 15 @@ Char.of_int_exn ((r4 lsr 8) land 255);
  Bytes.unsafe_to_string ~no_mutation_while_string_reachable:u

let to_string ?(upper = false) u =
  let hbase = if upper then 55 else 87 in
  let hex hbase i = Char.of_int_exn (if i < 10 then 48 + i else hbase + i) in
  let s = Bytes.of_string "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX" in
  let i = ref 0 in
  let j = ref 0 in
  let byte s i c =
    Bytes.set s i @@ hex hbase (c lsr 4);
    Bytes.set s (i + 1) @@ hex hbase (c land 15)
  in
  while !j < 4 do
    byte s !i (Char.to_int u.[!j]);
    i := !i + 2;
    Int.incr j
  done;
  Int.incr i;
  while !j < 6 do
    byte s !i (Char.to_int u.[!j]);
    i := !i + 2;
    Int.incr j
  done;
  Int.incr i;
  while !j < 8 do
    byte s !i (Char.to_int u.[!j]);
    i := !i + 2;
    Int.incr j
  done;
  Int.incr i;
  while !j < 10 do
    byte s !i (Char.to_int u.[!j]);
    i := !i + 2;
    Int.incr j
  done;
  Int.incr i;
  while !j < 16 do
    byte s !i (Char.to_int u.[!j]);
    i := !i + 2;
    Int.incr j
  done;
  Bytes.unsafe_to_string ~no_mutation_while_string_reachable:s

let pp ppf u = Caml.Format.pp_print_string ppf (to_string u)

let v4_gen seed =
  let rand = rand seed in
  function () -> v4_ocaml_random_uuid rand

let generateId () =
  let randomState = Random.State.default in
  v4_gen randomState ()
