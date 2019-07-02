#!/usr/bin/env bash
echo "PlayerHasRegisterd"; cat data/0.json | jq '[.[] | select(.type == "PlayerHasRegistered") | .payload.owner_id] | length'
cat data/0.json | jq '[.[] | select(.type == "QuizWasCreated") | .payload.owner_id] | sort | length'
cat data/0.json | jq '[.[] | select(.type == "QuizWasCreated") | .payload.owner_id] | sort| unique | length'
cat data/0.json | jq '[.[] | select(.type == "GameWasOpened")] | length'
echo "PlayerJoinedGame"; cat data/0.json | jq '[.[] | select(.type == "PlayerJoinedGame")] | length'
