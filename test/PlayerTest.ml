open TestFramework
open Lib
;;describe "player answers questions"
    (fun { test } ->
       let open Distribution in
         let id = Uuid.generateId () in
         let createQuizDistribution =
           MonthDistribution.Never |> MonthDistribution.create in
         let joinGameDistribution =
           ((MonthDistribution.ForEver
               (((Steady (((PerMonth (10))[@explicit_arity ])))
                 [@explicit_arity ])))
             [@explicit_arity ]) |> MonthDistribution.create in
         let openGameDistribution =
           MonthDistribution.Never |> MonthDistribution.create in
         let create answerType =
           World.Player.create ~id ~answerType ~createQuizDistribution
             ~joinGameDistribution ~openGameDistribution in
         test "fast correct player"
           (fun { expect } ->
              let answerType =
                { PlayerType.delay = 0.; delayRange = 0.; correctness = 1. } in
              let player = create answerType in
              expect.equal (`AnswerCorrectly 0.)
                (World.Player.answerQuestion player));
         test "fast incorrect player"
           (fun { expect } ->
              let answerType =
                { PlayerType.delay = 0.; delayRange = 0.; correctness = 0. } in
              let player = create answerType in
              expect.equal (`AnswerIncorrectly 0.)
                (World.Player.answerQuestion player));
         test "slow player"
           (fun { expect } ->
              let answerType =
                { PlayerType.delay = 1.; delayRange = 0.; correctness = 0. } in
              let player = create answerType in
              expect.equal `AnswerTimeout
                (World.Player.answerQuestion player)))