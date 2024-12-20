SELECT * 
FROM (
    SELECT
        p.position AS position,
        c.champion_name AS champion_name,
        ((SUM(s.kills) + SUM(s.assists)) / SUM(s.deaths)) AS kda
    FROM stat AS s
    JOIN participant AS p ON s.player_id = p.player_id
    JOIN champ AS c ON c.champion_id = p.champion_id
    GROUP BY c.champion_id, p.position
    HAVING SUM(s.deaths) > 0
    ORDER BY position, kda DESC
) AS tmp
WHERE position IN ('DUO_CARRY', 'DUO_SUPPORT', 'JUNGLE', 'MID', 'TOP') 
GROUP BY position
ORDER BY position;