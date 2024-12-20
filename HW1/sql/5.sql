select champion_name 
from champ
where champion_id not in (
    select distinct champ.champion_id
    from teamban 
    join match_info on teamban.match_id=match_info.match_id
    join champ on teamban.champion_id = champ.champion_id
    where SUBSTRING_INDEX(m.version, '.', 2)='7.7'
    )
    order by champion_name;