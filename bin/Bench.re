/* open Core; */
/* open Core_bench.Std; */
/*  */
/* let timestamp = 1560715876.; */
/*  */
/* let memory = Bytes.of_string("2000-00-00T00:00:00Z"); */
/* let set = (pos, int) => Bytes.unsafe_set(memory, pos, (Char.of_int_exn (48+int))); */
/*  */
/* let print = ({tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec}: Unix.tm) => { */
/*   set(2, (tm_year/10 mod 10)); */
/*   set(3, (tm_year mod 10)); */
/*   set(5, ((tm_mon + 1)/10 mod 10)); */
/*   set(6, ((tm_mon + 1) mod 10)); */
/*   set(8, ((tm_mday)/10 mod 10)); */
/*   set(9, ((tm_mday) mod 10)); */
/*   set(11, ((tm_hour)/10 mod 10)); */
/*   set(12, ((tm_hour) mod 10)); */
/*   set(14, ((tm_min)/10 mod 10)); */
/*   set(15, ((tm_min) mod 10)); */
/*   set(17, ((tm_sec)/10 mod 10)); */
/*   set(18, ((tm_sec) mod 10)); */
/*   Bytes.to_string(memory) */
/* }; */
/*  */
/* Console.log("HERE"); */
/* let coredate = Unix.gmtime(timestamp); */
/* Console.log(print(coredate)); */
/* Console.log("Running Test Program:"); */
/*  */
/* let format = ({tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec}: Unix.tm) => { */
/*   Printf.sprintf( */
/*     "%d-%02d-%02dT%02d:%02d:%02dZ", */
/*     1900 + tm_year, */
/*     tm_mon + 1, */
/*     tm_mday, */
/*     tm_hour, */
/*     tm_min, */
/*     tm_sec, */
/*   ); */
/* }; */
/*  */
/* let main = () => { */
/*   let uuid = Lib.Uuid.generateId(); */
/*   let date = CalendarLib.Calendar.from_unixfloat(timestamp); */
/*   let pdate = Ptime.of_float_s(timestamp); */
/*   let pdate = Option.value_exn(pdate); */
/*   let coreDate = Unix.gmtime(timestamp); */
/*   Console.log(Ptime.to_rfc3339(pdate, ~tz_offset_s=0)); */
/*   Console.log(Unix.strftime(coreDate, "%FT%TZ")); */
/*   Console.log(format(coreDate)); */
/*   Command.run( */
/*     Bench.make_command([ */
/*       Bench.Test.create(~name="simulation", () => ignore(Lib.Stream.hello())), */
/*       /* Bench.Test.create(~name="uuid", () => ignore(Lib.Uuid.to_string(uuid))), */ */
/*       /* Bench.Test.create(~name="toTimestamp", () => ignore(Lib.Events.toTimestamp(9000239))), */ */
/*       /* Bench.Test.create(~name="float_of_int", () => ignore(float_of_int(timestamp)), */ */
/*       /* Bench.Test.create(~name="from_unixfloat calendar", () => ignore(CalendarLib.Calendar.Precise.from_unixfloat(timestamp))), */ */
/*       /* Bench.Test.create(~name="from_unixfloat ptime", () => ignore(Ptime.of_float_s(timestamp))), */ */
/*       /* Bench.Test.create(~name="from_unixfloat core", () => ignore(Unix.gmtime(timestamp))), */ */
/*       /* Bench.Test.create(~name="format Calendar", () => ignore(CalendarLib.Printer.Calendar.sprint("%iT%T%z", date))), */ */
/*       /* Bench.Test.create(~name="format Ptime", () => */ */
/*       /*   ignore(Ptime.to_rfc3339(pdate, ~tz_offset_s=0)) */ */
/*       /* ), */ */
/*       /* Bench.Test.create(~name="format Unix", () => */ */
/*       /*   ignore(Unix.strftime(coreDate, "%FT%T%z")) */ */
/*       /* ), */ */
/*       /* /* Bench.Test.create(~name="format Printf", () => ignore(format(coreDate))), */ */ */
/*       /* Bench.Test.create(~name="format Bytes", () => ignore(print(coreDate))), */ */
/*       /* Bench.Test.create(~name="format", () => ignore(CalendarLib.Printer.Calendar.sprint("%i", date))), */ */
/*       /* Bench.Test.create(~name="format", () => ignore(CalendarLib.Printer.Calendar.sprint("%T", date))), */ */
/*       /* Bench.Test.create(~name="format", () => ignore(CalendarLib.Printer.Calendar.sprint("%z", date))), */ */
/*     ]), */
/*   ); */
/* }; */
/*  */
/* let () = main(); */
