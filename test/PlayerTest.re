open TestFramework;
open Lib;

describe("player answers questions", ({test}) => {
  open Distribution;
  let id = Uuid.generateId();
  let createQuizDistribution =
    MonthDistribution.Never |> MonthDistribution.create;
  let joinGameDistribution =
    MonthDistribution.ForEver(Steady(PerMonth(10)))
    |> MonthDistribution.create;
  let openGameDistribution =
    MonthDistribution.Never
    |> MonthDistribution.create;

  let create = answerType =>
    World.Player.create(
      ~id,
      ~answerType,
      ~createQuizDistribution,
      ~joinGameDistribution,
      ~openGameDistribution
    );

  test("fast correct player", ({expect}) => {
    let answerType = {PlayerType.delay: 0., delayRange: 0., correctness: 1.};
    let player = create(answerType);

    expect.equal(`AnswerCorrectly(0.), World.Player.answerQuestion(player));
  });
  test("fast incorrect player", ({expect}) => {
    let answerType = {PlayerType.delay: 0., delayRange: 0., correctness: 0.};
    let player = create(answerType);

    expect.equal(
      `AnswerIncorrectly(0.),
      World.Player.answerQuestion(player),
    );
  });

  test("slow player", ({expect}) => {
    let answerType = {PlayerType.delay: 1., delayRange: 0., correctness: 0.};
    let player = create(answerType);

    expect.equal(`AnswerTimeout, World.Player.answerQuestion(player));
  });
});
