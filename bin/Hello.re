let createDir = (dir) => {
	let _ = try (Core.Unix.mkdir(dir)) {
		| Unix.Unix_error(Unix.EEXIST, _, _)	 => ()
	};
	()
};

Console.log("Running Test Program:");
let () = createDir("data");
let () = Lib.Stream.hello();
