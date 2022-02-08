Random.full_init [| 1; 2; 3 |]

let createDir dir =
  let _ =
    try Unix.mkdir dir 0o740
    with ((Unix.Unix_error (Unix.EEXIST, _, _)) [@explicit_arity]) -> ()
  in
  ()
;;

Stdio.print_endline "Running Generator"

let () = createDir "data";;

Stdio.print_endline "Generating basic file..."

let () = Lib.Stream.basic ();;

Stdio.print_endline "Generating full file...";;
Stdio.print_endline "Hang on, this might take a bit"

let () = Lib.Stream.full ();;

Stdio.print_endline "Done"
