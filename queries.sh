#!/usr/bin/env bash
cat data/0.json | jq '[.[] | select(.type == "QuizWasCreated") | .payload.owner_id] | sort | length'
cat data/0.json | jq '[.[] | select(.type == "QuizWasCreated") | .payload.owner_id] | sort| unique | length'
cat data/0.json | jq '[.[] | select(.type == "GameWasOpened")] | length'
cat data/0.json | jq '[.[] | select(.type == "PlayerHasRegistered") | .payload.owner_id] | length'
