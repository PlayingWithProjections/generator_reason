type t = {
  startTimestamp: float ;
  endTimestamp: float }
let daysUntilNow ~days  =
  let open CalendarLib.Calendar in
    let period = CalendarLib.Calendar.Precise.Period.day days in
    let now = Precise.make 2019 09 20 10 00 0 in
    let start = Precise.add now (Precise.Period.opp period) in
    {
      startTimestamp = (Precise.to_unixfloat start);
      endTimestamp = (Precise.to_unixfloat now)
    }