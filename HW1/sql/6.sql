SELECT position, champion_name
FROM (
   SELECT 
       p.position AS position,
       COUNT(*) AS cnt,
       c.champion_name AS champion_name
   FROM participant AS p
   JOIN stat AS s
       ON s.player_id = p.player_id
   JOIN match_info AS m
       ON p.match_id = m.match_id
   JOIN champ AS c
       ON c.champion_id = p.champion_id
   WHERE m.duration BETWEEN 2400 AND 3000
   GROUP BY p.position, p.champion_id
   ORDER BY position, cnt DESC
) AS tmp
WHERE position IN ('DUO_CARRY', 'DUO_SUPPORT', 'JUNGLE', 'MID', 'TOP') 
GROUP BY position
ORDER BY position;