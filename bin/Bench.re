open Core;
open Core_bench.Std;

let main = () => {
	let uuid = Lib.Uuid.generateId();
	let date = CalendarLib.Calendar.from_unixfloat(1000329.);
	let pdate = Ptime.of_float_s(1000329.);
	let pdate = Option.value_exn(pdate);
	let coreDate = Unix.gmtime(1000329.);
	Console.log(Ptime.to_rfc3339(pdate, ~tz_offset_s=0));
	Console.log(Unix.strftime(coreDate, "%FT%TZ"));
  Command.run(
    Bench.make_command([
      Bench.Test.create(~name="simulation", () => ignore(Lib.Stream.hello())),
      /* Bench.Test.create(~name="uuid", () => ignore(Lib.Uuid.to_string(uuid))), */
      /* Bench.Test.create(~name="toTimestamp", () => ignore(Lib.Events.toTimestamp(1000239))), */
      /* Bench.Test.create(~name="float_of_int", () => ignore(float_of_int(1000329))), */
      /* Bench.Test.create(~name="from_unixfloat calendar", () => ignore(CalendarLib.Calendar.Precise.from_unixfloat(1000329.))), */
      /* Bench.Test.create(~name="from_unixfloat ptime", () => ignore(Ptime.of_float_s(1000329.))), */
      /* Bench.Test.create(~name="from_unixfloat core", () => ignore(Unix.gmtime(1000329.))), */
      /* Bench.Test.create(~name="format calendar", () => ignore(CalendarLib.Printer.Calendar.sprint("%iT%T%z", date))), */
      /* Bench.Test.create(~name="format ptime", () => ignore(Ptime.to_rfc3339(pdate, ~tz_offset_s=0))), */
      /* Bench.Test.create(~name="format core", () => ignore(Unix.strftime(coreDate, "%FT%T%z"))), */
      /* Bench.Test.create(~name="format", () => ignore(CalendarLib.Printer.Calendar.sprint("%i", date))), */
      /* Bench.Test.create(~name="format", () => ignore(CalendarLib.Printer.Calendar.sprint("%T", date))), */
      /* Bench.Test.create(~name="format", () => ignore(CalendarLib.Printer.Calendar.sprint("%z", date))), */
    ]),
  );
};

let () = main();
