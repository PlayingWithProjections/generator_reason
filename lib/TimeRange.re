type t = {
  startTimestamp: float,
  endTimestamp: float,
};

let daysUntilNow = (~days) => {
  open CalendarLib.Calendar;
  let period = CalendarLib.Calendar.Precise.Period.day(days);
  let now = Precise.make(2019, 09, 20, 10, 00, 0);
  let start = Precise.add(now, Precise.Period.opp(period));
  {
    startTimestamp: Precise.to_unixfloat(start),
    endTimestamp: Precise.to_unixfloat(now),
  };
};
