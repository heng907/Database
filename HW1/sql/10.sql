SELECT
    c.champion_name AS self_champ_name,
    SUM(target.team=0) / COUNT(*) AS win_ratio,
    (SUM(s.kills) + SUM(s.assists)) / SUM(s.deaths) AS self_kda,
    AVG(s.goldearned) AS seld_avg_gold,
    'Renekton' AS enemy_role,
    (SUM(target.kills) + SUM(target.assists)) / SUM(target.deaths) AS enemy_kills,
    AVG(target.goldearned) AS enemy_avg_gold,
    count(*) AS battle_record
FROM participant AS p
JOIN stat AS s ON p.player_id = s.player_id
JOIN champ AS c ON c.champion_id = p.champion_id
JOIN (
    SELECT 
        p.match_id AS match_id, 
        s.win AS team,
        s.kills AS kills,
        s.assists AS assists,
        s.deaths AS deaths,
        s.goldearned AS goldearned
    FROM stat AS s
    JOIN participant AS p ON s.player_id = p.player_id
    WHERE p.champion_id = 58 AND
        p.position = 'TOP' AND
        s.deaths > 0
) AS target
    ON p.match_id = target.match_id AND
        1 - s.win = target.team
GROUP BY p.champion_id
HAVING SUM(s.deaths) > 0 AND
    SUM(target.deaths) > 0 AND
    battle_record > 100
ORDER BY battle_record DESC
LIMIT 5;