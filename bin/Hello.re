let createDir = (dir) => {
	let _ = try (Unix.mkdir(dir, 0o640)) {
		| Unix.Unix_error(Unix.EEXIST, _, _)	 => ()
	};
	()
};

Console.log("Running Test Program:");
let () = createDir("data");
let () = Lib.Stream.hello();
