let createDir = dir => {
  let _ =
    try(Unix.mkdir(dir, 0o740)) {
    | Unix.Unix_error(Unix.EEXIST, _, _) => ()
    };
  ();
};

Console.log("Running Generator");
let () = createDir("data");
Console.log("Generating basic file...");
let () = Lib.Stream.basic();
Console.log("Generating full file...");
Console.log("Hang on, this might take a bit");
Console.log("While waiting, you can start the excercises with the basic file");
let () = Lib.Stream.full();
Console.log("Done");
