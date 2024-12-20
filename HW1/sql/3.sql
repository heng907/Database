SELECT champ.champion_name, COUNT(*) AS cnt FROM champ
join participant on participant.champion_id=champ.champion_id
where participant.position="JUNGLE"
group by champion_name
order by cnt desc limit 3;