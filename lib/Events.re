[@deriving show]
type event =
  | PlayerHasRegistered{
      playerId: Uuid.t,
      lastName: string,
      firstName: string,
      timeStamp: int,
    }
  | QuizWasCreated{
      id: Uuid.t,
      ownerId: Uuid.t,
      timeStamp: int,
      quizTitle: string,
    }
  | QuestionAddedToQuiz{
      id: Uuid.t,
      quizId: Uuid.t,
      question: string,
      answer: string,
      timeStamp: int,
    }
  | QuizWasPublished{
      id: Uuid.t,
      quizId: Uuid.t,
      timeStamp: int,
    }
  | GameWasOpened{
      quizId: Uuid.t,
      gameId: Uuid.t,
    }
  | PlayerJoinedGame{
      playerId: Uuid.t,
      gameId: Uuid.t,
    }
  | MinuteHasPassed{timeStamp: int};
