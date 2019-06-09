open Core.Std;
open Core_bench.Std;

let main = () => {
  Command.run(
    Bench.make_command([
      Bench.Test.create(~name="simulation", () => ignore(Lib.Stream.hello())),
    ]),
  );
};

let () = main();
