open Core.Std;
open Core_bench.Std;

let main = () => {
  Random.self_init();
  let x = Random.float(10.0);
  let y = Random.float(10.0);
  Command.run(
    Bench.make_command([
      Bench.Test.create(~name="Float add", () => ignore(x +. y)),
      Bench.Test.create(~name="Float mul", () => ignore(x *. y)),
      Bench.Test.create(~name="Float div", () => ignore(x /. y)),
    ]),
  );
};

let () = main();
