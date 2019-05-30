[@deriving show]
type event =
  | PlayerHasRegistered{
      playerId: Uuidm.t,
      lastName: string,
      firstName: string,
      timeStamp: int,
    }
  | QuizWasCreated{
      id: Uuidm.t,
      ownerId: Uuidm.t,
      timeStamp: int,
      quizTitle: string,
    }
  | QuestionAddToQuiz{
      id: Uuidm.t,
      quizId: Uuidm.t,
      question: string,
      answer: string,
      timeStamp: int,
    }
  | QuizWasPublished{
      id: Uuidm.t,
      quizId: Uuidm.t,
      timeStamp: int,
    }
  | GameWasOpened{
      quizId: Uuidm.t,
      gameId: Uuidm.t,
    }
  | PlayerJoinedGame{
      playerId: Uuidm.t,
      gameId: Uuidm.t,
    }
  | MinuteHasPassed{timeStamp: int};
