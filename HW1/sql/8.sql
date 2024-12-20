SELECT champion_name FROM champ
WHERE champion_id NOT IN (
    SELECT DISTINCT c.champion_id
    FROM teamban AS t
    JOIN match_info AS m ON t.match_id = m.match_id
    JOIN champ AS c on t.champion_id = c.champion_id
    WHERE SUBSTRING_INDEX(m.version, '.', 2) = '7.7'
)
ORDER BY champion_name;