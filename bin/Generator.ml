Random.full_init [| 1; 2; 3 |]

let createDir dir =
  let _ =
    try Unix.mkdir dir 0o740
    with ((Unix.Unix_error (Unix.EEXIST, _, _)) [@explicit_arity]) -> ()
  in
  ()
;;

Console.log "Running Generator"

let () = createDir "data";;

Console.log "Generating basic file..."

let () = Lib.Stream.basic ();;

Console.log "Generating full file...";;
Console.log "Hang on, this might take a bit"

let () = Lib.Stream.full ();;

Console.log "Done"
