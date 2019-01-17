--
-- PostgreSQL database dump
--

-- Dumped from database version 10.6 (Ubuntu 10.6-0ubuntu0.18.04.1)
-- Dumped by pg_dump version 10.6 (Ubuntu 10.6-0ubuntu0.18.04.1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: DATABASE "Little_River"; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON DATABASE "Little_River" IS 'Main database for the farm.';


--
-- Name: alpro; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA alpro;


--
-- Name: SCHEMA alpro; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA alpro IS 'holds info from DeLaval Alpro system.';


--
-- Name: bas; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA bas;


--
-- Name: SCHEMA bas; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA bas IS 'building automation systems';


--
-- Name: batch; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA batch;


--
-- Name: SCHEMA batch; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA batch IS 'Holds tables of batch data, where query is so long to run that it has to be run by a cron job.';


--
-- Name: bovinemanagement; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA bovinemanagement;


--
-- Name: SCHEMA bovinemanagement; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA bovinemanagement IS 'Standard public schema';


--
-- Name: cropping; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA cropping;


--
-- Name: SCHEMA cropping; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA cropping IS 'new schema for cropping stuff.';


--
-- Name: documents; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA documents;


--
-- Name: SCHEMA documents; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA documents IS 'holds various farm documents.';


--
-- Name: gis; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA gis;


--
-- Name: SCHEMA gis; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA gis IS 'Stores all the postgis 2.1 stuff.';


--
-- Name: hr; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA hr;


--
-- Name: SCHEMA hr; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA hr IS 'store human resources info';


--
-- Name: intwebsite; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA intwebsite;


--
-- Name: SCHEMA intwebsite; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA intwebsite IS 'Standard public schema';


--
-- Name: machinery; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA machinery;


--
-- Name: SCHEMA machinery; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA machinery IS 'Outside barn equipment';


--
-- Name: misc; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA misc;


--
-- Name: SCHEMA misc; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA misc IS 'misc stuff';


--
-- Name: nutrition; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA nutrition;


--
-- Name: SCHEMA nutrition; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA nutrition IS 'stores information on harvested crops and nutrition.';


--
-- Name: pgagent; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA pgagent;


--
-- Name: SCHEMA pgagent; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA pgagent IS 'pgAgent system tables';


--
-- Name: picture; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA picture;


--
-- Name: SCHEMA picture; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA picture IS 'stores pictures';


--
-- Name: system; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA system;


--
-- Name: SCHEMA system; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA system IS 'holds things like the table update log';


--
-- Name: topology; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA topology;


--
-- Name: SCHEMA topology; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA topology IS 'PostGIS Topology schema';


--
-- Name: urban_feeder_foreign_tiere; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA urban_feeder_foreign_tiere;


--
-- Name: SCHEMA urban_feeder_foreign_tiere; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA urban_feeder_foreign_tiere IS 'urban feeder tiere schema foreign link';


--
-- Name: wcauthentication; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA wcauthentication;


--
-- Name: SCHEMA wcauthentication; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA wcauthentication IS 'Standard public schema';


--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


--
-- Name: adminpack; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS adminpack WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION adminpack; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION adminpack IS 'administrative functions for PostgreSQL';


--
-- Name: pgagent; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS pgagent WITH SCHEMA pgagent;


--
-- Name: EXTENSION pgagent; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION pgagent IS 'A PostgreSQL job scheduler';


--
-- Name: postgis; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS postgis WITH SCHEMA gis;


--
-- Name: EXTENSION postgis; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION postgis IS 'PostGIS geometry, geography, and raster spatial types and functions';


--
-- Name: postgis_topology; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS postgis_topology WITH SCHEMA topology;


--
-- Name: EXTENSION postgis_topology; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION postgis_topology IS 'PostGIS topology spatial types and functions';


--
-- Name: postgres_fdw; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS postgres_fdw WITH SCHEMA gis;


--
-- Name: EXTENSION postgres_fdw; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION postgres_fdw IS 'foreign-data wrapper for remote PostgreSQL servers';


--
-- Name: alpro_production_for_group_at_date_result; Type: TYPE; Schema: alpro; Owner: -
--

CREATE TYPE alpro.alpro_production_for_group_at_date_result AS (
	timestamp_of_interest timestamp without time zone,
	location_id integer,
	avg_milk_production numeric,
	total_milk_production numeric
);


--
-- Name: latest_full_day_production_result; Type: TYPE; Schema: alpro; Owner: -
--

CREATE TYPE alpro.latest_full_day_production_result AS (
	date date,
	milkyield numeric
);


--
-- Name: milk_components_result; Type: TYPE; Schema: batch; Owner: -
--

CREATE TYPE batch.milk_components_result AS (
	test_date date,
	fat_percentage numeric,
	protein_percentage numeric,
	lactose_percentage numeric,
	ssc numeric,
	mun numeric
);


--
-- Name: valacta_components_for_group_at_date_result; Type: TYPE; Schema: batch; Owner: -
--

CREATE TYPE batch.valacta_components_for_group_at_date_result AS (
	timestamp_of_interest timestamp without time zone,
	location_id integer,
	avg_fat_per numeric,
	avg_prot_per numeric,
	avg_lactose_per numeric
);


--
-- Name: bovine_log; Type: TYPE; Schema: bovinemanagement; Owner: -
--

CREATE TYPE bovinemanagement.bovine_log AS (
	type text,
	type_id integer,
	event_time timestamp without time zone,
	days_ago interval,
	text text,
	extra1 text
);


--
-- Name: dup_result2; Type: TYPE; Schema: bovinemanagement; Owner: -
--

CREATE TYPE bovinemanagement.dup_result2 AS (
	f1 integer,
	f2 text
);


--
-- Name: milkandbeefwithholding_result; Type: TYPE; Schema: bovinemanagement; Owner: -
--

CREATE TYPE bovinemanagement.milkandbeefwithholding_result AS (
	bovine_index integer,
	milk_withholding timestamp without time zone,
	beef_withholding timestamp without time zone
);


--
-- Name: base_saturation_result; Type: TYPE; Schema: cropping; Owner: -
--

CREATE TYPE cropping.base_saturation_result AS (
	cec numeric,
	ca_base_sat numeric,
	mg_base_sat numeric,
	k_base_sat numeric,
	na_base_sat numeric,
	h_base_sat numeric,
	total_base_sat numeric
);


--
-- Name: cutting_yield; Type: TYPE; Schema: cropping; Owner: -
--

CREATE TYPE cropping.cutting_yield AS (
	field_id integer,
	alpha_numeric_id character varying,
	year integer,
	field_ha numeric,
	feed_type_name text,
	diameter_foot numeric,
	yield_bag_feet numeric,
	linear_yield numeric
);


--
-- Name: geometry_dump; Type: TYPE; Schema: nutrition; Owner: -
--

CREATE TYPE nutrition.geometry_dump AS (
	path integer[]
);


--
-- Name: ingredient_feedcurr_breakout_result; Type: TYPE; Schema: nutrition; Owner: -
--

CREATE TYPE nutrition.ingredient_feedcurr_breakout_result AS (
	table_name text,
	table_id integer
);


--
-- Name: ingredient_type_result; Type: TYPE; Schema: nutrition; Owner: -
--

CREATE TYPE nutrition.ingredient_type_result AS (
	feed_type_id integer,
	name text
);


--
-- Name: mix_info_result; Type: TYPE; Schema: nutrition; Owner: -
--

CREATE TYPE nutrition.mix_info_result AS (
	mix_id integer,
	recipe_id integer,
	recipe_start_timestamp timestamp without time zone,
	recipe_stop_timestamp timestamp without time zone,
	timestamp_of_interest timestamp without time zone,
	location_id integer,
	location_name text,
	location_count integer,
	mix_cost_per_cow numeric,
	mix_kg_day_dry_per_cow numeric,
	mix_as_fed_kg_per_cow numeric,
	mix_moisture_percent numeric,
	mix_modify numeric,
	total_mix_as_fed_kg numeric
);


--
-- Name: mix_recipe_item_info_result; Type: TYPE; Schema: nutrition; Owner: -
--

CREATE TYPE nutrition.mix_recipe_item_info_result AS (
	recipe_item_id integer,
	recipe_weighted_count numeric,
	kg_day_dry numeric,
	dry_matter_percent numeric,
	recipe_ingredient_wet_amount numeric,
	recipe_ingredient_dry_amount numeric
);


--
-- Name: recipe_per_cow_info_result; Type: TYPE; Schema: nutrition; Owner: -
--

CREATE TYPE nutrition.recipe_per_cow_info_result AS (
	recipe_id integer,
	recipe_cost_per_cow numeric,
	recipe_kg_day_dry_per_cow numeric,
	recipe_as_fed_kg_per_cow numeric,
	recipe_moisture_percent numeric
);


--
-- Name: alpro_mpcnumber_to_human_readable(integer); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.alpro_mpcnumber_to_human_readable(func_mpcnumber integer) RETURNS text
    LANGUAGE plpgsql
    AS $$BEGIN

CASE
    WHEN func_mpcnumber=90 THEN
      return 'L01';
   WHEN func_mpcnumber=91 THEN
      return 'L02';
      WHEN func_mpcnumber=92 THEN
      return 'L03';
   WHEN func_mpcnumber=93 THEN
      return 'L04';
   WHEN func_mpcnumber=94 THEN
      return 'L05';
   WHEN func_mpcnumber=95 THEN
      return 'L06';
   WHEN func_mpcnumber=96 THEN
      return 'L07';
   WHEN func_mpcnumber=97 THEN
      return 'L08';
   WHEN func_mpcnumber=98 THEN
      return 'L09';
   WHEN func_mpcnumber=99 THEN
      return 'L10';
 WHEN func_mpcnumber=80 THEN
      return 'R01';
   WHEN func_mpcnumber=81 THEN
      return 'R02';
      WHEN func_mpcnumber=82 THEN
      return 'R03';
   WHEN func_mpcnumber=83 THEN
      return 'R04';
   WHEN func_mpcnumber=84 THEN
      return 'R05';
   WHEN func_mpcnumber=85 THEN
      return 'R06';
   WHEN func_mpcnumber=86 THEN
      return 'R07';
   WHEN func_mpcnumber=87 THEN
      return 'R08';
   WHEN func_mpcnumber=88 THEN
      return 'R09';
   WHEN func_mpcnumber=89 THEN
      return 'R10';
   ELSE
     return null;
END CASE;

END;$$;


--
-- Name: alpro_production_for_group_at_date(integer, timestamp without time zone); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.alpro_production_for_group_at_date(func_location_id integer, func_timestamp_of_interest timestamp without time zone) RETURNS SETOF alpro.alpro_production_for_group_at_date_result
    LANGUAGE plpgsql
    AS $$/*
finds the total milk production of a group by summing up cows production on a specific date 
*/



DECLARE
answer numeric;

BEGIN

/* the average milk production for cow only works because we assume alpro never records 0 production */

RETURN QUERY 
WITH temp as (
SELECT bovine_id,date,milking_number,milkyield FROM alpro.cow WHERE date=func_timestamp_of_interest::date
), temp2 as (
SELECT * FROM temp
LEFT JOIN bovinemanagement.location_list_at_timestamp(func_location_id, func_timestamp_of_interest::date) ON location_list_at_timestamp=temp.bovine_id WHERE location_list_at_timestamp IS NOT NULL
)
SELECT func_timestamp_of_interest,func_location_id,(sum(milkyield)/count(milkyield))*2,sum(milkyield) FROM temp2;

END;$$;


--
-- Name: alpro_production_saleable_milk(date, integer); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.alpro_production_saleable_milk(func_date date, func_milking_number integer) RETURNS numeric
    LANGUAGE plpgsql
    AS $$DECLARE
answer numeric;

BEGIN

/* find the milk yield for all the milking groups based on there location at time of milking */

with temp as (
SELECT bovine_id,milkyield,milktime,bovinemanagement.location_id_at_timestamp(bovine_id,milktime) as location_id FROM alpro.cow 
WHERE date=func_date::date AND milking_number=func_milking_number
) 
SELECT INTO ANSWER sum(milkyield) FROM temp WHERE location_id=40 OR location_id=41 OR location_id=42;

return ANSWER;

END;$$;


--
-- Name: bovine_at_sort_gate_at_specifc_time(timestamp without time zone); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.bovine_at_sort_gate_at_specifc_time(func_timestamp timestamp without time zone) RETURNS integer[]
    LANGUAGE sql
    AS $$SELECT array_agg (bovine_id)  as possible_bovine_id FROM alpro.sort_gate_log WHERE  event_time-interval '1 minute' <= func_timestamp AND event_time+interval '1 minute' >= func_timestamp$$;


--
-- Name: FUNCTION bovine_at_sort_gate_at_specifc_time(func_timestamp timestamp without time zone); Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON FUNCTION alpro.bovine_at_sort_gate_at_specifc_time(func_timestamp timestamp without time zone) IS 'used with estrus detector to correlate when cow passed through gate';


--
-- Name: create_time_column(); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.create_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
	BEGIN
	   NEW.create_time = now(); 
	   RETURN NEW;
	END;
	$$;


--
-- Name: latest_full_day_production(integer); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.latest_full_day_production(integer) RETURNS alpro.latest_full_day_production_result
    LANGUAGE sql
    AS $_$/* picks the latest date where there was a milking 1 and milking 2 on same day (hence the intersect) */
SELECT date,sum(milkyield) FROM alpro.cow WHERE bovine_id=$1 AND date=(
SELECT max(date) FROM (
SELECT (date) FROM alpro.cow WHERE bovine_id=$1 AND milking_number=1 
INTERSECT
SELECT (date) FROM alpro.cow WHERE bovine_id=$1 AND milking_number=2
) as ttt)
GROUP BY date;

$_$;


--
-- Name: milkingnumber_to_human_readable(integer); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.milkingnumber_to_human_readable(func_mikingnumber integer) RETURNS text
    LANGUAGE plpgsql
    AS $$BEGIN

CASE
    WHEN func_mikingnumber=1 THEN
      return 'AM';
   WHEN func_mikingnumber=2 THEN
      return 'PM';
   ELSE
     return null;
END CASE;

END;$$;


--
-- Name: FUNCTION milkingnumber_to_human_readable(func_mikingnumber integer); Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON FUNCTION alpro.milkingnumber_to_human_readable(func_mikingnumber integer) IS 'returns am or pm, assumes 2x';


--
-- Name: one_to_seven_day_milking_ratio(integer); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.one_to_seven_day_milking_ratio(func_bovine_id integer) RETURNS numeric
    LANGUAGE sql IMMUTABLE
    AS $$SELECT ((SELECT sum(milkyield)/count(milktime) from alpro.cow WHERE bovine_id= func_bovine_id AND milktime >= current_timestamp - interval '1 days') / (SELECT sum(milkyield)/count(milktime) from alpro.cow WHERE bovine_id=func_bovine_id AND milktime >= current_timestamp - interval '7 days')) as one_to_seven_ratio
$$;


--
-- Name: FUNCTION one_to_seven_day_milking_ratio(func_bovine_id integer); Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON FUNCTION alpro.one_to_seven_day_milking_ratio(func_bovine_id integer) IS 'given a bovine_id it returns the ratio of the last days milk to the last seven days milk. Used to look for cows down in milk. This function is smart and if they didn''t get milked it corrects for that.';


--
-- Name: stamp_update_log(); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.stamp_update_log() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN

BEGIN
      INSERT INTO system.update_log VALUES (TG_TABLE_SCHEMA,TG_TABLE_NAME);

EXCEPTION WHEN unique_violation THEN

      UPDATE system.update_log SET last_update='now()' WHERE schema_name=TG_TABLE_SCHEMA AND table_name=TG_TABLE_NAME;

END;

RETURN NEW;

END;

$$;


--
-- Name: FUNCTION stamp_update_log(); Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON FUNCTION alpro.stamp_update_log() IS 'a table calls this function via trigger whenever it is modified and it updates the timestamp log';


--
-- Name: update_time_column(); Type: FUNCTION; Schema: alpro; Owner: -
--

CREATE FUNCTION alpro.update_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN
	   NEW.update_time = now(); 
	   RETURN NEW;
	END;$$;


--
-- Name: bovine_milking_number_for_date(date); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.bovine_milking_number_for_date(func_date date) RETURNS TABLE(event_date date, sum_dim double precision, total_milking bigint, total_milking_high bigint, total_milking_low bigint, total_milking_heifer bigint)
    LANGUAGE sql
    AS $$

with temp as (
    SELECT local_number,d1.event_time,name,milking_location,location.id as loc_id,date_part('day',func_date-fresh_date) as dim from bovinemanagement.location_event d1 
JOIN bovinemanagement.location ON d1.location_id = location.id
JOIN bovinemanagement.bovine ON d1.bovine_id=bovine.id
JOIN bovinemanagement.lactation ON d1.bovine_id=lactation.bovine_id
WHERE d1.id in
(select id FROM bovinemanagement.location_event d2 WHERE d2.bovine_id = d1.bovine_id AND event_time <= func_date ORDER BY event_time DESC LIMIT 1) 
AND fresh_date = (select fresh_date FROM bovinemanagement.lactation d3 WHERE d3.bovine_id = d1.bovine_id AND fresh_date <= func_date ORDER BY fresh_date DESC LIMIT 1) 
AND (bovine.death_date IS NULL OR bovine.death_date > func_date)
AND milking_location=true 
ORDER BY d1.bovine_id asc
) 
SELECT func_date as event_date,sum(dim) as sum_dim, count(local_number) as total_milking,
(SELECT count FROM (SELECT count(temp.local_number) ,temp.loc_id FROM temp group by temp.loc_id) x40 WHERE loc_id=40) as total_milking_high,
(SELECT count FROM (SELECT count(temp.local_number) ,temp.loc_id FROM temp group by temp.loc_id) x41 WHERE loc_id=41) as total_milking_low,
(SELECT count FROM (SELECT count(temp.local_number) ,temp.loc_id FROM temp group by temp.loc_id) x42 WHERE loc_id=42)as total_milking_heifer
FROM temp  limit 1 

$$;


--
-- Name: calculate_milk_revenue(numeric, numeric, numeric, numeric, date); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.calculate_milk_revenue(func_liters numeric, func_butterfat numeric, func_protein numeric, func_lactose numeric, func_date date) RETURNS numeric
    LANGUAGE plpgsql
    AS $$/*
inputs PERCENT butterfat,protein,etc.
returns the amount of money made from shipping a certain amount of milk at a specific time.
*/



DECLARE

    rate_butterfat   NUMERIC;
    rate_protein     NUMERIC;
    rate_lactose     NUMERIC;
    rate_transport   NUMERIC;
    rate_promotion   NUMERIC;
    rate_admin       NUMERIC;
    rate_lab         NUMERIC;
    rate_research    NUMERIC; 
     /* optionally used */
    min_statement_date DATE; 
    max_statement_date DATE; 
     /* temp variables for math */
percent_SNF NUMERIC; 
density_milk_20c NUMERIC; 
solid_butterfat NUMERIC; 
solid_protein NUMERIC; 
solid_lactose NUMERIC; 
total_solids NUMERIC; 
revenue_butterfat NUMERIC; 
revenue_protein NUMERIC; 
revenue_lactose NUMERIC; 
total_revenue NUMERIC; 
expense_transport NUMERIC; 
expense_promotion NUMERIC; 
expense_admin NUMERIC; 
expense_lab NUMERIC; 
expense_research NUMERIC; 
total_expenses NUMERIC;

BEGIN

func_date=date_trunc('month',func_date);
 
/* if we have a statement for the month that the date called for wants, then this will work. otherwise.... */   
SELECT INTO rate_butterfat,rate_protein,rate_lactose,rate_transport,rate_promotion,rate_admin,rate_lab,rate_research butterfat,protein,lactose,transport,promotion,admin,lab,research FROM batch.milk_statement WHERE func_date = milk_statement.date LIMIT 1;
/* check if the date is older than the oldest statement, if so use that statement */
IF NOT FOUND THEN

   SELECT INTO min_statement_date min(date) FROM batch.milk_statement;
   SELECT INTO max_statement_date max(date) FROM batch.milk_statement;
   
   /* use the oldest statement to estimate the amount */
   IF (min_statement_date < func_date) THEN
   SELECT INTO rate_butterfat,rate_protein,rate_lactose,rate_transport,rate_promotion,rate_admin,rate_lab,rate_research butterfat,protein,lactose,transport,promotion,admin,lab,research FROM batch.milk_statement WHERE date=min_statement_date LIMIT 1;
   ELSE
   /* use the newest statement to project in the future. */
   SELECT INTO rate_butterfat,rate_protein,rate_lactose,rate_transport,rate_promotion,rate_admin,rate_lab,rate_research butterfat,protein,lactose,transport,promotion,admin,lab,research FROM batch.milk_statement WHERE date=max_statement_date LIMIT 1;
   END IF; 

END IF;

/* so now we are guaranteed to have some rate information from a statement, so do the math */

/*calculate density of milk because inputs are percent, not kg/hl*/
percent_SNF:=func_protein+func_lactose;
density_milk_20c:=100/((func_butterfat/0.93)+(percent_SNF/1.608)+(100-func_butterfat-percent_SNF));  /*answer is g/ml or kg/hl */


/* solids */

solid_butterfat:=func_liters*(func_butterfat*.01*density_milk_20c);
solid_protein:=func_liters*(func_protein*.01*density_milk_20c);
solid_lactose:=func_liters*(func_lactose*.01*density_milk_20c);

total_solids:=solid_butterfat+solid_protein+solid_lactose;

/* revenue */

revenue_butterfat:=solid_butterfat*rate_butterfat;
revenue_protein:=solid_protein*rate_protein;
revenue_lactose:=solid_lactose*rate_lactose;

total_revenue:=revenue_butterfat+revenue_protein+revenue_lactose;

/* expenses */



expense_transport:= rate_transport * func_liters; 
expense_promotion:= total_solids * rate_promotion; 
expense_admin:= total_solids * rate_admin; 
expense_lab:=  total_solids * rate_lab; 
expense_research:=  total_solids * rate_research; 

/* totals */
total_expenses:=expense_transport+expense_promotion+expense_admin+expense_lab+expense_research;

return total_revenue-total_expenses;

END;$$;


--
-- Name: calculate_milk_revenue_per_cow_per_date(integer, date); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.calculate_milk_revenue_per_cow_per_date(func_bovine_id integer, func_date date) RETURNS numeric
    LANGUAGE plpgsql
    AS $$/* calculates milk revenue for a specific date and specific cow*/
/* needs to know how many milking numbers the farm is currently using */



BEGIN

return batch.calculate_milk_revenue_per_cow_per_date_and_milking_number(func_bovine_id, func_date,1) + batch.calculate_milk_revenue_per_cow_per_date_and_milking_number(func_bovine_id, func_date,2);


END;$$;


--
-- Name: FUNCTION calculate_milk_revenue_per_cow_per_date(func_bovine_id integer, func_date date); Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON FUNCTION batch.calculate_milk_revenue_per_cow_per_date(func_bovine_id integer, func_date date) IS 'NOTE: if we go 3x this has to change!!!';


--
-- Name: calculate_milk_revenue_per_cow_per_date_and_milking_number(integer, date, integer); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.calculate_milk_revenue_per_cow_per_date_and_milking_number(func_bovine_id integer, func_date date, func_milking_number integer) RETURNS numeric
    LANGUAGE plpgsql
    AS $$/* calculates milk revenue for a specific date and specific cow for the specified single milking. Assumes lactose+other solids is 5.7% because valacta does not measure that. */

DECLARE
output numeric;

BEGIN

With temp as (
SELECT local_number,(SELECT (milkyield) FROM alpro.cow WHERE bovine_id=func_bovine_id AND date=func_date AND milking_number=func_milking_number) as milk_total,test_date,fat_per,prot_per 
FROM bovinemanagement.bovine 
LEFT JOIN batch.valacta_data ON valacta_data.chain=bovine.local_number
WHERE bovine.id=func_bovine_id AND fat_per is not null and prot_per is not null AND fat_per !=0 AND prot_per !=0
),
temp2 as (
SELECT test_date,abs(test_date-func_date) FROM temp GROUP BY temp.test_date ORDER by abs LIMIT 1
),
temp3 as (
SELECT * FROM temp WHERE temp.test_date=(SELECT test_date FROM temp2)
)
SELECT INTO output batch.calculate_milk_revenue (temp3.milk_total::numeric, temp3.fat_per::numeric, temp3.prot_per::numeric, 5.7::numeric, func_date) FROM temp3;

return output;
END;$$;


--
-- Name: FUNCTION calculate_milk_revenue_per_cow_per_date_and_milking_number(func_bovine_id integer, func_date date, func_milking_number integer); Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON FUNCTION batch.calculate_milk_revenue_per_cow_per_date_and_milking_number(func_bovine_id integer, func_date date, func_milking_number integer) IS 'calculates milk revenue for specific cow/date/milking #. Needed so that null is returned when no data is found so upstream calculations that average are correct.';


--
-- Name: create_time_column(); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.create_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN
	   NEW.create_time = now(); 
	   RETURN NEW;
	END;$$;


--
-- Name: current_lactation_total_revenue(integer); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.current_lactation_total_revenue(func_bovine_id integer) RETURNS numeric
    LANGUAGE plpgsql
    AS $$DECLARE 

output NUMERIC;

BEGIN

/* break it down to just one cow and one lactation */		
WITH lact as (
SELECT date,milking_number,milkyield
FROM alpro.cow 
WHERE bovine_id=func_bovine_id 
AND cow.date >= (SELECT fresh_date FROM bovinemanagement.lactationcurr WHERE bovine_id=func_bovine_id) 
AND ( cow.date <= (SELECT dry_date FROM bovinemanagement.lactationcurr WHERE bovine_id=func_bovine_id) OR  cow.date <= now())
),
/* now we have list of both milking yields for a day and fresh_date*/
daay as (
SELECT DISTINCT ON (lact.date) lact.date,lact.milking_number,lact.milkyield,lact2.milking_number as milking_number2,lact2.milkyield as milkyield2, (SELECT bovinemanagement.round_to_nearest_date(fresh_date) FROM bovinemanagement.lactationcurr WHERE bovine_id=func_bovine_id) as fresh_date
FROM lact
LEFT JOIN lact as lact2 on lact.date=lact2.date WHERE lact2.milking_number != lact.milking_number
),
/* show dim and total daily milk yield  and get valacta milk test data*/
comp as (
SELECT date,abs(fresh_date - date) as dim,milkyield+milkyield2 as dailymilkyield,
(batch.milk_components(func_bovine_id,date)) as components 
FROM daay
),
/* add in calculation from components to do revenue calculation */
rev as (
SELECT *,batch.calculate_milk_revenue (dailymilkyield, (components).fat_percentage, (components).protein_percentage, (components).lactose_percentage, date) FROM comp
)
SELECT INTO output round(sum(calculate_milk_revenue),2) as total_revenue FROM rev;

return output;

END;$$;


--
-- Name: milk_components(integer, date); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.milk_components(func_bovine_id integer, func_date date) RETURNS batch.milk_components_result
    LANGUAGE plpgsql
    AS $$/* takes bovine id and a date and returns you the closest milk components. Unfortunately valacta does not supply lactose numbers, so we guess. but lactose is really ~ 4.7% plus other solids, which gets to like 5.75. So for payment we really want lactose plus other solids*/


DECLARE
out_test_date DATE;
out_fat_per NUMERIC;
out_prot_per NUMERIC;
out_lactose_per NUMERIC;
out_mun NUMERIC;
out_ssc NUMERIC;
output batch.milk_components_result;
BEGIN

/* return milk components on a specific date */
SELECT INTO out_test_date,out_fat_per,out_prot_per,out_lactose_per,out_mun,out_ssc
test_date,fat_per,prot_per,5.75 as lact_per, mun/10 as mun,ssc/1000 as ssc FROM batch.valacta_data WHERE reg=(SELECT full_reg_number FROM bovinemanagement.bovine WHERE id=func_bovine_id) AND test_date=(
SELECT test_date FROM (
SELECT test_date,min(abs(test_date-func_date)) FROM batch.valacta_data WHERE reg=(SELECT full_reg_number FROM bovinemanagement.bovine WHERE id=func_bovine_id) GROUP BY test_date ORDER BY min ASC LIMIT 1) as sub);



output:= (out_test_date,out_fat_per,out_prot_per,out_lactose_per,out_mun,out_ssc);
return output;
END;
$$;


--
-- Name: prodoll_birthyear_quintile_rank(integer, integer); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.prodoll_birthyear_quintile_rank(func_bovine_id integer, func_year integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE
output integer;

BEGIN

with temp as (
select (EXTRACT(YEAR FROM birth_date)) as birth_year, bovine_id,
       prodoll, 
       ntile(5) over (partition by (EXTRACT(YEAR FROM birth_date)) order by prodoll asc) percentile
FROM batch.aggregate_view_curr
WHERE prodoll is not NULL AND (EXTRACT(YEAR FROM birth_date))=func_year
ORDER BY birth_year, percentile
) select INTO output percentile from temp WHERE bovine_id=func_bovine_id;
return output;
END;$$;


--
-- Name: FUNCTION prodoll_birthyear_quintile_rank(func_bovine_id integer, func_year integer); Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON FUNCTION batch.prodoll_birthyear_quintile_rank(func_bovine_id integer, func_year integer) IS 'returns prodoll quintile rank for a given birth year. Be careful to include correct birth year for bovine_id';


--
-- Name: quota_incentive_amount_on_date(date); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.quota_incentive_amount_on_date(func_date date) RETURNS numeric
    LANGUAGE plpgsql
    AS $$DECLARE
output numeric;
days numeric;
days_in_month numeric;
BEGIN

/* find number of incentive days on date of interest by looking at that month*/
SELECT incentive from batch.incentive_day WHERE date=(date_trunc('month', func_date)) INTO days;

/* when no incentive days, return 0 */
IF (days is null) THEN
output:=0;

/* when incentive days multiply by quota at that point in time and divide by days in the month. */
ELSE
SELECT DATE_PART('days', DATE_TRUNC('month', NOW()) + '1 MONTH'::INTERVAL - '1 day'::interval  ) into days_in_month;
SELECT into output round((batch.quota_on_date(func_date)*days)/days_in_month,3);
END IF;

return output;
END;$$;


--
-- Name: FUNCTION quota_incentive_amount_on_date(func_date date); Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON FUNCTION batch.quota_incentive_amount_on_date(func_date date) IS 'returns the amount of incentive portion of the quota for a specific date (day).';


--
-- Name: quota_on_date(date); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.quota_on_date(func_date date) RETURNS numeric
    LANGUAGE plpgsql
    AS $$/* takes a date and looks up how much BF milk quota there was on that paticular date. */
/* note quota stays the same for a month, so we will just truncate */

DECLARE
output numeric;
BEGIN


/* have to truncate to 1st and then increase by 1 day, because of the way adjustments, etc are stored 1 second after midnight on the 1st of the month */
SELECT INTO output sum(butterfat_change) from batch.quota WHERE event_time < (date_trunc('month',func_date) + interval '1 day')::date;

return output;
END;$$;


--
-- Name: FUNCTION quota_on_date(func_date date); Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON FUNCTION batch.quota_on_date(func_date date) IS 'calculates the actual daily BF quota on any given date.';


--
-- Name: quota_total_on_date(date); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.quota_total_on_date(func_date date) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
/* takes incentive plus quota for any date and returns it */

DECLARE
one numeric;
two numeric;
output numeric;
BEGIN

SELECT INTO one batch.quota_on_date(func_date);
SELECT INTO two batch.quota_incentive_amount_on_date(func_date);
output:=one+two;

return output;
END;$$;


--
-- Name: FUNCTION quota_total_on_date(func_date date); Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON FUNCTION batch.quota_total_on_date(func_date date) IS 'shows total quota on any date (incentives+actual)';


--
-- Name: update_time_column(); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.update_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN
	   NEW.update_time = now(); 
	   RETURN NEW;
	END;$$;


--
-- Name: valacta_components_for_group_at_date(integer, timestamp without time zone); Type: FUNCTION; Schema: batch; Owner: -
--

CREATE FUNCTION batch.valacta_components_for_group_at_date(func_location_id integer, func_timestamp_of_interest timestamp without time zone) RETURNS SETOF batch.valacta_components_for_group_at_date_result
    LANGUAGE plpgsql
    AS $$/*
finds the average components for all the cows in a group on a specific date 
*/

BEGIN

return query
WITH temp as (
SELECT DISTINCT ON (reg) reg,test_date,fat_per,prot_per,lactose_per FROM batch.valacta_data WHERE test_date <= func_timestamp_of_interest ORDER BY reg,test_date DESC
), temp2 as(
SELECT bovine.id,location_list_at_timestamp,reg,test_date,fat_per,prot_per,lactose_per FROM temp
LEFT JOIN bovinemanagement.bovine ON bovine.full_reg_number=temp.reg
LEFT JOIN bovinemanagement.location_list_at_timestamp(func_location_id,func_timestamp_of_interest) ON location_list_at_timestamp=bovine.id WHERE location_list_at_timestamp IS NOT NULL
)
SELECT func_timestamp_of_interest::timestamp without time zone as test_date, func_location_id::integer as location_id,avg(fat_per)::numeric as avg_fat_per,avg(prot_per)::numeric as avg_prot_per,avg(lactose_per)::numeric as avg_lactose_per FROM temp2;

END;$$;


--
-- Name: calculate_breeding_voluntary_waiting_period(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.calculate_breeding_voluntary_waiting_period(func_bovine_id integer) RETURNS date
    LANGUAGE plpgsql
    AS $$DECLARE 
   birth_date date;
   lactation_count integer;
   reg_number character varying(18);
   latest_fresh_date date;
   previous_fresh_date date;
   latest_milk_test_kg double precision;
   previous_milk_test_kg double precision;
   extra_days_to_wait integer; 
   age_at_calving integer;
   too_young_at_calving_extra_days integer;
   heifer_calf_initial_breeding_wait_days integer DEFAULT 395;  /* change to 480 when we want to not have many bred, (ie june) */ 
   heifer_initial_breeding_wait_days  integer DEFAULT 75;
   cow_initial_breeding_wait_days  integer DEFAULT 50;
BEGIN
/* Fill in some variables */
    SELECT INTO birth_date bovine.birth_date FROM  bovinemanagement.bovine WHERE id=func_bovine_id LIMIT 1;
    SELECT INTO reg_number bovine.full_reg_number FROM  bovinemanagement.bovine WHERE id=func_bovine_id LIMIT 1;
    SELECT INTO latest_fresh_date fresh_date FROM  bovinemanagement.lactation WHERE bovine_id=func_bovine_id Order By fresh_date DESC OFFSET 0 LIMIT 1;
    SELECT INTO previous_fresh_date fresh_date FROM  bovinemanagement.lactation WHERE bovine_id=func_bovine_id Order By fresh_date DESC OFFSET 1 LIMIT 1;
    SELECT INTO lactation_count COUNT(*) FROM  bovinemanagement.lactation WHERE bovine_id=func_bovine_id LIMIT 1;
    
/* latest_fresh_date := null; */

/*
RAISE NOTICE 'Initial value of birth_date =%',birth_date;
RAISE NOTICE 'Initial value of reg_number =%',reg_number;
RAISE NOTICE 'Initial value of latest_fresh_date =%',latest_fresh_date;
RAISE NOTICE 'Initial value of previous_fresh_date =%',previous_fresh_date;
RAISE NOTICE 'Initial value of lactation_count =%',lactation_count;
*/

/* Determine if we have a heifer calf, milking heifer, or cow. */
IF lactation_count = 0 
THEN      /* we have an unbred heifer calf and thus just use number of days since birth */
    RETURN  birth_date + (heifer_calf_initial_breeding_wait_days::varchar  || ' days'::varchar)::interval; 

/* we have a first lactation heifer */
ELSEIF  lactation_count = 1
THEN  
/* loop up her last test and get the milk mass in kg */
 SELECT INTO latest_milk_test_kg total_milk FROM  batch.valacta_data WHERE reg=reg_number AND fresh=latest_fresh_date AND total_milk !=0 ORDER BY test_date DESC LIMIT 1; 
          IF latest_milk_test_kg  >= 30
             THEN
               extra_days_to_wait := round(latest_milk_test_kg - 30) + heifer_initial_breeding_wait_days;
          ELSE 
               extra_days_to_wait :=   heifer_initial_breeding_wait_days;
          END IF;

/* check to see if they calved out too young < 25 months */
          too_young_at_calving_extra_days := 0;
       /*   age_at_calving := DATE_PART('days', latest_fresh_date - birth_date);
          IF (age_at_calving - heifer_ideal_calving_age_days) < 0
              THEN
                too_young_at_calving_extra_days = round(abs((age_at_calving - heifer_ideal_calving_age_days)*.3));
          END IF; */

/* now add days to wait to the current lactation fresh date, to give us a breed after date */
RETURN latest_fresh_date + (extra_days_to_wait::varchar  || ' days'::varchar)::interval + (too_young_at_calving_extra_days::varchar  || ' days'::varchar)::interval; 
     
/* we have a milking cow */
ELSEIF  lactation_count >= 2
THEN 
/*RAISE NOTICE 'XX lactation_count =%',lactation_count;*/
SELECT INTO previous_milk_test_kg total_milk FROM  batch.valacta_data WHERE reg=reg_number AND fresh=previous_fresh_date AND total_milk !=0  ORDER BY abs(test_date-fresh-290) ASC LIMIT 1; 

/* add 1 day for every kg over 10 kg from previous test. */
	    IF previous_milk_test_kg  >= 10
	     THEN
               extra_days_to_wait = round(previous_milk_test_kg - 10) + cow_initial_breeding_wait_days;
          ELSE 
               extra_days_to_wait =   cow_initial_breeding_wait_days;
          END IF;

   RETURN latest_fresh_date  + (extra_days_to_wait::varchar  || ' days'::varchar)::interval; 
ELSE
/* Should throw an error here instead of null*/
return '0001-01-01';
END IF;

END;$$;


--
-- Name: calculate_number_of_breeding_since_fresh(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.calculate_number_of_breeding_since_fresh(func_breeding_event_id integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$/* Given a breeding, calculate its number since birth or fresh */
/* returns the count for the breeding */
DECLARE 
nolacs INTEGER;
answer INTEGER;
t_bovine_id integer;
t_breed_time timestamp without time zone;
BEGIN

/* find out actual breeding time */
SELECT actual_breeding_time,bovine_id FROM bovinemanagement.breeding_event WHERE id=func_breeding_event_id INTO t_breed_time,t_bovine_id ;

/* find if they are heifer or cow */
SELECT count(fresh_date) FROM bovinemanagement.lactation WHERE fresh_date <= t_breed_time AND bovine_id=t_bovine_id INTO nolacs;

IF (nolacs = 0) THEN
/* this is for heifers, ie bovines who have not calved, so counts since birth and STOP at current breeding time*/
SELECT count(id) FROM bovinemanagement.breeding_view WHERE actual_breeding_time <= t_breed_time AND  actual_breeding_time >= (SELECT birth_date FROM bovinemanagement.bovine WHERE id=t_bovine_id) AND bovine_id=t_bovine_id INTO answer;

ELSE
/* this is for cows who have calved, so counts breedings since last fresh date  and STOP at current breeding time*/
SELECT count(id) FROM bovinemanagement.breeding_view WHERE actual_breeding_time <= t_breed_time AND actual_breeding_time >= (SELECT max(fresh_date) FROM bovinemanagement.lactation WHERE bovine_id=t_bovine_id AND fresh_date <=t_breed_time ) AND bovine_id=t_bovine_id  INTO answer;

END IF;

  

        RETURN answer;
END;$$;


--
-- Name: FUNCTION calculate_number_of_breeding_since_fresh(func_breeding_event_id integer); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.calculate_number_of_breeding_since_fresh(func_breeding_event_id integer) IS 'Given a breeding, calculate its number since birth or fresh ';


--
-- Name: calculate_number_of_recent_breedings(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.calculate_number_of_recent_breedings(func_bovine_id integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$/* this counts the number of breedings since birth or last calving depending on animal. */
DECLARE 
nolacs INTEGER;
answer INTEGER;
BEGIN

SELECT count(fresh_date) FROM bovinemanagement.lactation WHERE bovine_id=func_bovine_id INTO nolacs;

IF (nolacs = 0) THEN
/* this is for heifers, ie bovines who have not calved */
SELECT count(id) FROM bovinemanagement.breeding_view WHERE actual_breeding_time >= (SELECT birth_date FROM bovinemanagement.bovine WHERE id=func_bovine_id) AND bovine_id=func_bovine_id INTO answer;

ELSE
/* this is for cows who have calved, so counts breedings since last fresh date */
SELECT count(id) FROM bovinemanagement.breeding_view WHERE actual_breeding_time >= (SELECT max(fresh_date) FROM bovinemanagement.lactation WHERE bovine_id=func_bovine_id) AND bovine_id=func_bovine_id  INTO answer;

END IF;

  

        RETURN answer;
END;



$$;


--
-- Name: FUNCTION calculate_number_of_recent_breedings(func_bovine_id integer); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.calculate_number_of_recent_breedings(func_bovine_id integer) IS 'this counts the number of breedings since birth or last calving depending on animal.';


--
-- Name: calculate_number_of_recent_breedings_embyro_implants(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.calculate_number_of_recent_breedings_embyro_implants(func_bovine_id integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$/* this counts the number of breedings and embyro implants since birth or last calving depending on animal. SInce implants, applies to recipient only*/
DECLARE 
nolacs INTEGER;
answer INTEGER;
BEGIN

SELECT count(fresh_date) FROM bovinemanagement.lactation WHERE bovine_id=func_bovine_id INTO nolacs;

IF (nolacs = 0) THEN
/* this is for heifers, ie bovines who have not calved */
SELECT count(id) FROM bovinemanagement.combined_breeding_embryo_view  WHERE event_time >= (SELECT birth_date FROM bovinemanagement.bovine WHERE id=func_bovine_id) AND recipient_bovine_id=func_bovine_id INTO answer;

ELSE
/* this is for cows who have calved, so counts breedings since last fresh date */
SELECT count(id) FROM bovinemanagement.combined_breeding_embryo_view WHERE event_time >= (SELECT max(fresh_date) FROM bovinemanagement.lactation WHERE bovine_id=func_bovine_id) AND recipient_bovine_id=func_bovine_id  INTO answer;

END IF;

  

        RETURN answer;
END;
$$;


--
-- Name: FUNCTION calculate_number_of_recent_breedings_embyro_implants(func_bovine_id integer); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.calculate_number_of_recent_breedings_embyro_implants(func_bovine_id integer) IS 'this counts the number of breedings and embyro implants since birth or last calving depending on animal.';


--
-- Name: calculate_number_of_recent_embyro_implants(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.calculate_number_of_recent_embyro_implants(func_bovine_id integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$/* this counts the number of embyro implants since birth or last calving depending on animal. Since implants, applies to recipient only*/
DECLARE 
nolacs INTEGER;
answer INTEGER;
BEGIN

SELECT count(fresh_date) FROM bovinemanagement.lactation WHERE bovine_id=func_bovine_id INTO nolacs;

IF (nolacs = 0) THEN
/* this is for heifers, ie bovines who have not calved */
SELECT count(id) FROM bovinemanagement.embryo_view  WHERE event_time >= (SELECT birth_date FROM bovinemanagement.bovine WHERE id=func_bovine_id) AND recipient_bovine_id=func_bovine_id INTO answer;

ELSE
/* this is for cows who have calved, so counts breedings since last fresh date */
SELECT count(id) FROM bovinemanagement.embryo_view WHERE event_time >= (SELECT max(fresh_date) FROM bovinemanagement.lactation WHERE bovine_id=func_bovine_id) AND recipient_bovine_id=func_bovine_id  INTO answer;

END IF;
RETURN answer;
END;
$$;


--
-- Name: FUNCTION calculate_number_of_recent_embyro_implants(func_bovine_id integer); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.calculate_number_of_recent_embyro_implants(func_bovine_id integer) IS 'only does embryo implants since fresh/born';


--
-- Name: check_semen_inventory_for_straw(character); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.check_semen_inventory_for_straw(func_semen_code character) RETURNS numeric
    LANGUAGE sql
    AS $_$SELECT count(semen_straw.id)::numeric as straw_count FROM  bovinemanagement.semen_straw
LEFT JOIN  bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON  sire_semen_code.sire_full_reg_number=sire.full_reg_number
WHERE semen_straw.semen_code=$1 AND breeding_event_id is null AND reserved=false and discarded=false  and bin IS NOT NULL and tank <> 'Z'::bpchar$_$;


--
-- Name: FUNCTION check_semen_inventory_for_straw(func_semen_code character); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.check_semen_inventory_for_straw(func_semen_code character) IS 'returns the inventory count for a given semen straw by returning the number in stock with same semen code.';


--
-- Name: create_time_column(); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.create_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
	BEGIN
	   NEW.create_time = now(); 
	   RETURN NEW;
	END;
	$$;


--
-- Name: donotbreed_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.donotbreed_at_timestamp(func_bovine_id integer, func_event_time timestamp without time zone) RETURNS boolean
    LANGUAGE plpgsql
    AS $$DECLARE 
output BOOLEAN;

/* gets the the next donotbreed status for a cow after the date of interest. */
BEGIN

SELECT INTO output donotbreed FROM bovinemanagement.service_picks WHERE event_time <= func_event_time AND bovine_id=func_bovine_id ORDER BY event_time DESC LIMIT 1;

return output;

END;$$;


--
-- Name: due_date(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.due_date(func_bovine_id integer) RETURNS timestamp without time zone
    LANGUAGE plpgsql
    AS $$DECLARE 
tablename text;
tableindex integer;
ret_date timestamp without time zone;
temp_fresh_date timestamp without time zone;

/* TODO rule out ones that have just started a lacation */

/* Gets a single bovines due date */
BEGIN

SELECT INTO tablename (regexp_split_to_array(estimated_breeding_embryo_event_id, E'\\|+'))[1]::text
   FROM bovinemanagement.preg_check_event
   WHERE preg_check_event.bovine_id = func_bovine_id AND preg_check_event.event_time = (( SELECT max(preg_check_event.event_time) AS max FROM bovinemanagement.preg_check_event WHERE preg_check_event.bovine_id = func_bovine_id));

/* second time is because we do not know the syntax to declare an array */
SELECT INTO tableindex (regexp_split_to_array(estimated_breeding_embryo_event_id, E'\\|+'))[2]::integer
   FROM bovinemanagement.preg_check_event
   WHERE preg_check_event.bovine_id = func_bovine_id AND preg_check_event.event_time = (( SELECT max(preg_check_event.event_time) AS max FROM bovinemanagement.preg_check_event WHERE preg_check_event.bovine_id = func_bovine_id));

/* depending on the method used, we have to do different queries form embryos and breaddings */
IF tablename = 'breeding' THEN
    SELECT INTO ret_date (actual_breeding_time + '280 days 12:00:00'::interval)  FROM bovinemanagement.breeding_event WHERE id=tableindex;
ELSEIF tablename = 'embryo' THEN
     SELECT INTO ret_date (embryo_implant.event_time + '273 days 12:00:00'::interval) FROM bovinemanagement.embryo_implant WHERE id=tableindex;
ELSE
ret_date:= null;
END IF;

/* if the date when the preg check breeding occurred is before they were fresh, it is invalid */
/* the 280 days is a fudge for embryos, but no preg checks are done in the first 7 days of lactation anyway */
SELECT INTO temp_fresh_date max(fresh_date) FROM bovinemanagement.lactation WHERE lactation.bovine_id=func_bovine_id;
IF (ret_date) < (temp_fresh_date + '50 days 12:00:00'::interval) THEN
ret_date:= null;
END IF;

return ret_date;

END;$$;


--
-- Name: lactation_number(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.lactation_number(func_bovine_id integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE

answer INTEGER;

BEGIN

Select into answer count(id) as lactation_count FROM bovinemanagement.lactation WHERE bovine_id=func_bovine_id;

return answer;

END;$$;


--
-- Name: last_breeding(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.last_breeding(func_bovine_id integer) RETURNS timestamp without time zone
    LANGUAGE plpgsql
    AS $$/* Returns the last time a cow since fresh or heifer has been bred. */
DECLARE
ret_last_breeding_date timestamp without time zone;
temp_fresh_date timestamp without time zone;


BEGIN


SELECT INTO temp_fresh_date max(fresh_date) FROM bovinemanagement.lactation WHERE lactation.bovine_id=func_bovine_id;

/* cow case */
IF (temp_fresh_date is not null) THEN
SELECT INTO ret_last_breeding_date max(actual_breeding_time) FROM bovinemanagement.breeding_event WHERE (breeding_event.bovine_id=func_bovine_id AND breeding_event.actual_breeding_time >= temp_fresh_date);
/* calf case */
ELSE
SELECT INTO ret_last_breeding_date max(actual_breeding_time) FROM bovinemanagement.breeding_event WHERE bovine_id=func_bovine_id;
END IF;

return ret_last_breeding_date;

END;$$;


--
-- Name: last_four_milkings_tstzrange(); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.last_four_milkings_tstzrange() RETURNS TABLE(prevmilking1 tstzrange, prevmilking2 tstzrange, prevmilking3 tstzrange, prevmilking4 tstzrange)
    LANGUAGE plpgsql IMMUTABLE
    AS $$
DECLARE
numberOfMilkings integer; 
 millking1_start timetz;
 millking1_stop timetz;
 millking2_start timetz;
 millking2_stop timetz;
 millking3_start timetz;
 millking3_stop timetz;
ret RECORD;
BEGIN
numberOfMilkings :=2;
 millking1_start := '04:30'::time with time zone;
 millking1_stop := '09:30'::time with time zone;
 millking2_start := '16:30'::time with time zone;
 millking2_stop := '21:30'::time with time zone;
 millking3_start := '0:00'::time with time zone;
 millking3_stop := '0:00'::time with time zone;

IF  numberOfMilkings =2 THEN

CASE
    WHEN  now()>= (current_date + millking2_stop)::timestamptz  THEN
    /* after 2nd milking  in same day*/
  RETURN QUERY SELECT
 (tstzrange((current_date - interval '0 day')::date + millking1_start,(current_date - interval '0 day')::date + millking1_stop)) ,
 (tstzrange((current_date - interval '0 day')::date + millking2_start,(current_date - interval '0 day')::date + millking2_stop)),
  (tstzrange((current_date - interval '1 day')::date + millking1_start,(current_date - interval '1 day')::date + millking1_stop)) ,
  (tstzrange((current_date - interval '1 day')::date + millking2_start,(current_date - interval '1 day')::date + millking2_stop)) ;
 WHEN  now() >= (current_date +  millking1_stop)::timestamptz  THEN
/* after first milking and before 2nd ends */
  RETURN QUERY SELECT
 (tstzrange((current_date - interval '0 day')::date + millking1_start,(current_date - interval '0 day')::date + millking1_stop)),
 ( tstzrange((current_date - interval '1 day')::date + millking2_start,(current_date - interval '1 day')::date + millking2_stop)),
 ( tstzrange((current_date - interval '1 day')::date + millking1_start,(current_date - interval '1 day')::date + millking1_stop)) ,
 (  tstzrange((current_date - interval '2 day')::date + millking2_start,(current_date - interval '2 day')::date + millking2_stop)) ;
 ELSE
/* before first milking of day*/
  RETURN QUERY  SELECT
 (  tstzrange((current_date - interval '1 day')::date + millking2_start,(current_date - interval '1 day')::date + millking2_stop)) ,
(  tstzrange((current_date - interval '1 day')::date + millking1_start,(current_date - interval '1 day')::date + millking1_stop) ),
 (  tstzrange((current_date - interval '2 day')::date + millking2_start,(current_date - interval '2 day')::date + millking2_stop)) ,
  (tstzrange((current_date - interval '2 day')::date + millking1_start,(current_date - interval '2 day')::date + millking1_stop)) ;


END CASE;



    END IF;




END;
$$;


--
-- Name: location_event_id_time_at_location(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.location_event_id_time_at_location(func_location_event_id integer) RETURNS interval
    LANGUAGE sql
    AS $$/* takes into account if the bovine is in the latest location it uses the current timestamp, and calculates to "now". uses a window function to create row numbers for temp table and that gives us temporal order */
with temp as (
SELECT row_number() OVER (),* FROM bovinemanagement.location_event WHERE bovine_id=(SELECT bovine_id FROM bovinemanagement.location_event WHERE id = func_location_event_id) ORDER BY event_time ASC
)
 SELECT (SELECT COALESCE((SELECT event_time FROM temp WHERE row_number=((SELECT row_number FROM temp WHERE id= func_location_event_id )-1)), current_timestamp)) - (SELECT event_time from temp where  id = func_location_event_id)$$;


--
-- Name: FUNCTION location_event_id_time_at_location(func_location_event_id integer); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.location_event_id_time_at_location(func_location_event_id integer) IS 'Calculates for a sing location_event_id who long the bovine was in that location. (ie how long until they moved to the next one.)';


--
-- Name: location_id_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.location_id_at_timestamp(func_bovine_id integer, func_event_time timestamp without time zone) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE 
output INTEGER;

/* gets the location of specific bovine at the date of interest, then returns location_id */   
BEGIN

SELECT INTO output location_id FROM bovinemanagement.location_event WHERE event_time <= func_event_time AND bovine_id=func_bovine_id ORDER BY event_time DESC LIMIT 1;

return output;

END;$$;


--
-- Name: location_list_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.location_list_at_timestamp(func_location_id integer, date_of_interest timestamp without time zone) RETURNS SETOF integer
    LANGUAGE plpgsql
    AS $$DECLARE 


/* gets the location of every bovine at the date of interest, then returns there bovine id's */   
BEGIN


return query

WITH temp_table as (
(SELECT DISTINCT ON (bovine_id) bovine_id,location_id FROM bovinemanagement.location_event WHERE event_time <= date_of_interest ORDER BY bovine_id,event_time DESC) 
)
 SELECT temp_table.bovine_id::integer FROM temp_table WHERE location_id=func_location_id;



END;
$$;


--
-- Name: location_name_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.location_name_at_timestamp(func_bovine_id integer, func_event_time timestamp without time zone) RETURNS character varying
    LANGUAGE plpgsql
    AS $$DECLARE 
output character varying;

/* gets the location of specific bovine at the date of interest, then returns location_id */   
BEGIN

SELECT INTO output name FROM bovinemanagement.location_event 
LEFT JOIN  bovinemanagement.location ON location.id=location_id 
WHERE event_time <= func_event_time AND bovine_id=func_bovine_id ORDER BY event_time DESC LIMIT 1;

return output;

END;$$;


--
-- Name: location_total_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.location_total_at_timestamp(func_location_id integer, date_of_interest timestamp without time zone) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE 
total integer;

/* gets the location of every bovine at the date of interest, then just pulls out the group total we want */   
BEGIN

WITH temp_table as (
(SELECT DISTINCT ON (bovine_id) location_id FROM bovinemanagement.location_event WHERE event_time <= date_of_interest ORDER BY bovine_id,event_time DESC) 
)
SELECT INTO total count(temp_table.location_id) FROM temp_table WHERE location_id=func_location_id;

return total;

END;
$$;


--
-- Name: milk_and_beef_withholding(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.milk_and_beef_withholding(integer) RETURNS bovinemanagement.milkandbeefwithholding_result
    LANGUAGE sql
    AS $_$/*for milk post calving, we look up the first fresh date after the medicine event and use it*/
WITH temp_table as (
SELECT fresh_date, bovine_id,event_time,trade_name, 
event_time + milk_withholding * INTERVAL '1 hour' as milk_withholding_until,  
event_time + beef_withholding * INTERVAL '1 day' as beef_withholding_until,
(SELECT min(fresh_date) as next_fresh_date FROM bovinemanagement.lactation WHERE bovine_id=$1 AND fresh_date >= event_time) + milk_post_calving_withholding * INTERVAL '1 hour' as milk_post_calving_withholding 
FROM bovinemanagement.medicine_administered  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id=bovinecurr.id
WHERE bovine_id=$1 AND event_time >= current_date-interval '100 days'
), temp_table2 as ( 
/* select worst case from all the medicines */
SELECT max(milk_withholding_until) as milk,max(beef_withholding_until) as beef,max(milk_post_calving_withholding) as post_calving FROM temp_table
), temp_table3 as ( 
/* allows us to select the max between milk withholding and milk post calving withholding, keeps beef separate */
SELECT milk,beef,post_calving,milk as booth FROM temp_table2
UNION 
SELECT milk,beef,post_calving,post_calving as booth FROM temp_table2
)
SELECT (SELECT temp_table.bovine_id FROM temp_table LIMIT 1) as bovine_id, max(booth) as milk_withholding, max(beef) as beef_withholding FROM temp_table3
$_$;


--
-- Name: pregnant_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.pregnant_at_timestamp(func_bovine_id integer, func_event_time timestamp without time zone) RETURNS text
    LANGUAGE plpgsql
    AS $$/* look at any animal and return whether they were pregnant at that moment in time */
/* assumes that there is a positive preg check for every calf born */

DECLARE 
f_birth_date date;
f_fresh_date timestamp without time zone;
f_start_date timestamp without time zone;
ret_value text;
temp_preg_check_result character varying(8);
temp_estimated_breeding_embryo_event_id text;
BEGIN

/* sets a starting point on when to look for preg checks in cows */
SELECT INTO f_fresh_date fresh_date FROM bovinemanagement.lactation WHERE bovine_id=func_bovine_id AND fresh_date <= func_event_time ORDER BY fresh_date DESC LIMIT 1;

/*use birthdate as the date to start looking for preg checks with heifers */
SELECT INTO f_birth_date birth_date FROM bovinemanagement.bovine WHERE id=func_bovine_id;

IF f_fresh_date != NULL THEN
f_start_date:= f_fresh_date;
ELSE
f_start_date:= f_birth_date::timestamp without time zone;
END IF;

/* now look at the last preg check and if it is positive return the id, otherwise return null */

SELECT INTO temp_preg_check_result,temp_estimated_breeding_embryo_event_id  preg_check_result,combined_breeding_embryo_view.id
FROM bovinemanagement.preg_check_event 
LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id=estimated_breeding_embryo_event_id	
WHERE bovine_id=func_bovine_id AND combined_breeding_embryo_view.event_time <@ tsrange(f_start_date,func_event_time) ORDER BY combined_breeding_embryo_view.event_time DESC limit 1;


IF temp_preg_check_result = 'pregnant' THEN
ret_value := temp_estimated_breeding_embryo_event_id;
ELSE
ret_value := null;
END IF;

return ret_value;

END;$$;


--
-- Name: reproduction_events(integer); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.reproduction_events(func_bovine_id integer) RETURNS SETOF bovinemanagement.bovine_log
    LANGUAGE plpgsql
    AS $$BEGIN
 RETURN QUERY

/************************* BREEDING  */
SELECT 'Breeding'::text  as type, breeding_event.id as type_id, actual_breeding_time as event_time,date_trunc('days',now()-actual_breeding_time) as days_ago,
concat(' #',bovinemanagement.calculate_number_of_breeding_since_fresh(breeding_event.id)::text  , ' to ', short_name, ' by ', trim(actual_breeding_userid) ,'. ', comment ) as text, sire_semen_code.semen_code::text as extra1
FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.semen_straw ON semen_straw.id = breeding_event.semen_straw_id
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN bovinemanagement.palpate_comment ON palpate_comment.id =  breeding_event.comment_id
WHERE bovine_id=func_bovine_id AND state_frozen=true and sexed_semen=false
UNION
/************************* BREEDING SEXED */
SELECT 'Breeding Sexed'::text  as type, breeding_event.id as type_id, actual_breeding_time as event_time,date_trunc('days',now()-actual_breeding_time) as days_ago,
concat(' #',bovinemanagement.calculate_number_of_breeding_since_fresh(breeding_event.id)::text  ,  ' to ', short_name, ' by ', trim(estimated_breeding_userid) ,'. ', comment ) as text, sire_semen_code.semen_code::text as extra1
FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.semen_straw ON semen_straw.id = breeding_event.semen_straw_id
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN bovinemanagement.palpate_comment ON palpate_comment.id =  breeding_event.comment_id
WHERE bovine_id=func_bovine_id AND state_frozen=true and sexed_semen=true
UNION
/************************* BREEDING SCHEDULED */
SELECT 'Breeding Scheduled'::text  as type, breeding_event.id as type_id, estimated_breeding_time as event_time,date_trunc('days',now()-estimated_breeding_time) as days_ago,
concat( 'to ', short_name, ' by ', trim(estimated_breeding_userid) ,'. ', comment ) as text, sire_semen_code.semen_code::text as extra1
FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.semen_straw ON semen_straw.id = breeding_event.semen_straw_id
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN bovinemanagement.palpate_comment ON palpate_comment.id =  breeding_event.comment_id
WHERE bovine_id=func_bovine_id AND state_frozen=false and sexed_semen=false
UNION
/************************* BREEDING SCHEDULED SEXED */
SELECT 'Breeding Scheduled'::text  as type, breeding_event.id as type_id, estimated_breeding_time as event_time,date_trunc('days',now()-estimated_breeding_time) as days_ago,
concat( 'to ', short_name, ' by ', trim(actual_breeding_userid) ,'. ', comment ) as text, sire_semen_code.semen_code::text as extra1
FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.semen_straw ON semen_straw.id = breeding_event.semen_straw_id
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN bovinemanagement.palpate_comment ON palpate_comment.id =  breeding_event.comment_id
WHERE bovine_id=func_bovine_id AND state_frozen=false and sexed_semen=true
UNION
/************************* HEAT */
SELECT 'Heat'::text  as type, estrus_type.id as type_id,event_time as event_time , date_trunc('days',now()-event_time) as days_ago, concat(name ,' as seen by ', trim(estrus_event.userid),'.') as text,null::text as extra1
FROM bovinemanagement.estrus_event
LEFT JOIN bovinemanagement.estrus_type ON estrus_event.estrus_type_id=estrus_type.id 
WHERE estrus_event.bovine_id=func_bovine_id 
UNION
/************************* KAMAR */        
SELECT 'Kamar'::text  as type, id as type_id, event_time,  date_trunc('days',now()-event_time) as days_ago, concat(trim(kamar_event_type),' as seen by ',trim(userid)) as text,null::text as extra1
FROM bovinemanagement.kamar_event WHERE bovine_id=func_bovine_id
UNION
/************************* PREG CHECK */    
SELECT 'Preg Check'::text  as type,preg_check_event.id as type_id,  preg_check_event.event_time,date_trunc('days',now()-preg_check_event.event_time) as days_ago,
concat(test_full_name,' performed with result of <b>',preg_check_result,'</b> ',('(to ' || TO_CHAR(combined_breeding_embryo_view.event_time, 'Mon dd, YYYY') ||') '),trim(preg_check_comment_custom),' as done by ',trim(userid),'.') as text,null::text as extra1
        FROM bovinemanagement.preg_check_event
LEFT JOIN bovinemanagement.preg_check_comment ON preg_check_comment.id=preg_check_event.preg_check_comment_id
LEFT JOIN bovinemanagement.preg_check_type ON preg_check_type.id=preg_check_event.preg_check_type_id
LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON preg_check_event.estimated_breeding_embryo_event_id=combined_breeding_embryo_view.id
WHERE bovine_id=func_bovine_id
UNION
/********************* CALF FEMALE */
SELECT 'Calf Female'::text  as type, calving_event.id as type_id, event_time as event_time, date_trunc('days',now()-event_time) as days_ago,
'# '|| concat(calf.local_number, ' - ' ||  bovinemanagement.short_name(calf.full_name)) as text,calf.id::text as extra1
FROM bovinemanagement.calving_event 
LEFT JOIN (SELECT local_number,id,full_reg_number,full_name,rfid_number FROM bovinemanagement.bovine) calf ON rfid_number=calving_event.calf_rfid_number::numeric
LEFT JOIN bovinemanagement.lactation ON lactation.id=lactation_id
WHERE bovine_id=func_bovine_id AND calf_sex='female'
UNION
/********************* CALF MALE */
/** NOTE: does not take into account males with purebred registration  and kept*/
SELECT 'Calf Male'::text  as type, calving_event.id as type_id, event_time as event_time, date_trunc('days',now()-event_time) as days_ago,
'# '|| calving_event.calf_rfid_number as text,null::text as extra1
FROM bovinemanagement.calving_event 
LEFT JOIN bovinemanagement.lactation ON lactation.id=lactation_id
WHERE bovine_id=func_bovine_id AND calf_sex='male'
UNION
/****************** EMBRYO IMPLANT */
/** NOTE: Does not support sexed semen embryo straws */
SELECT 
'Embryo Implant'::text  as type,  embryo_implant.id as type_id, event_time,date_trunc('days',now()-embryo_implant.event_time) as days_ago,
concat( 'Dam: ',donor_dam_full_reg_number, ' Sire: ', sire.short_name , ' by ', embryo_implant.implanter_userid) as text,null::text as extra1
   FROM bovinemanagement.embryo_implant
   LEFT JOIN bovinemanagement.bovinecurr ON embryo_implant.bovine_id = bovinecurr.id
   LEFT JOIN bovinemanagement.embryo_straw ON embryo_implant.embryo_straw_id = embryo_straw.id
   LEFT JOIN bovinemanagement.embryo_flush ON embryo_straw.embryo_flush_id = embryo_flush.id
   LEFT JOIN bovinemanagement.embryo_implant_comment ON embryo_implant_comment.id = embryo_implant.comment_id
   LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = embryo_straw.sire_full_reg_number
   LEFT JOIN bovinemanagement.bovinecurr bovinecurr2 ON embryo_flush.donor_dam_full_reg_number::text = bovinecurr.full_reg_number::text
WHERE embryo_implant.bovine_id =func_bovine_id
UNION
/****************** EMBRYO IMPLANT SCHEDULED */
SELECT 
'Embryo Implant Scheduled'::text  as type,  embryo_implant.id as type_id, estimated_event_time as event_time,date_trunc('days',now()-embryo_implant.estimated_event_time) as days_ago,
concat( 'Dam: ',donor_dam_full_reg_number, ' Sire: ', sire.short_name , ' by ', embryo_implant.estimated_event_userid	) as text,null::text as extra1
   FROM bovinemanagement.embryo_implant
   LEFT JOIN bovinemanagement.bovinecurr ON embryo_implant.bovine_id = bovinecurr.id
   LEFT JOIN bovinemanagement.embryo_straw ON embryo_implant.embryo_straw_id = embryo_straw.id
   LEFT JOIN bovinemanagement.embryo_flush ON embryo_straw.embryo_flush_id = embryo_flush.id
   LEFT JOIN bovinemanagement.embryo_implant_comment ON embryo_implant_comment.id = embryo_implant.comment_id
   LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = embryo_straw.sire_full_reg_number
   LEFT JOIN bovinemanagement.bovinecurr bovinecurr2 ON embryo_flush.donor_dam_full_reg_number::text = bovinecurr.full_reg_number::text
WHERE embryo_implant.bovine_id =func_bovine_id AND implanter_userid is null
UNION
/************************ ET (Recipient) Calf **/
/** NOTE: Supports male calves in bovine table, but ignores male ones never registered */
SELECT 
'ET Calf'::text  as type, bovine.id as type_id,  bovine.birth_date as event_time, date_trunc('days',now()-bovine.birth_date) as days_ago,
concat(bovine.local_number, ' - ' ||  bovinemanagement.short_name(bovine.full_name)) || ' carried by ' || concat(recip_bovine.local_number , ' - ' ||  bovinemanagement.short_name(recip_bovine.full_name )) as text,recip_bovine.id::text as extra1
FROM bovinemanagement.bovine 
LEFT JOIN bovinemanagement.bovine as recip_bovine ON recip_bovine.full_reg_number=bovine.recipient_full_reg_number
WHERE bovine.dam_full_reg_number=(SELECT full_reg_number FROM bovinemanagement.bovine WHERE id=func_bovine_id)
AND bovine.recipient_full_reg_number is not null
UNION
/****************** Lactation Start */
SELECT 'Lactation Start'::text  as type, id as type_id, fresh_date as event_time, date_trunc('days',now()-fresh_date) as days_ago,
concat('') as text, null::text as extra1
FROM bovinemanagement.lactation WHERE lactation.bovine_id=func_bovine_id
UNION
/***************** Reproduction Comment */
SELECT 
'Comment'::text  as type, id as type_id, event_time, date_trunc('days',now()-event_time) as days_ago, comment as text, null::text as extra1
FROM bovinemanagement.comment_custom WHERE type='reproductive' AND comment_custom.bovine_id=func_bovine_id
UNION
/****************** Repro Diagnosis */
SELECT 'Diagnosis'::text  as type, medical_diagnosis.id as type_id, event_time, date_trunc('days',now()-event_time) as days_ago, diagnosis as text, null::text as extra1
 FROM bovinemanagement.medical_diagnosis 
LEFT JOIN bovinemanagement.medical_diagnosis_type ON medical_diagnosis_type.id=medical_diagnosis.diagnosis_type_id 
WHERE bovine_id=func_bovine_id AND reproductive_related=true
UNION
/***************** Repro Protocol */
SELECT 	'Protocol'::text  as type, protocol_event.id as type_id, date_start as event_time, date_trunc('days',now()-date_start) as days_ago, ('Started ' || name) as text, null::text as extra1
FROM bovinemanagement.protocol_event
LEFT JOIN bovinemanagement.protocol_type ON protocol_type.id= protocol_event.protocol_type_id
WHERE bovine_id=func_bovine_id
/***************** Repro Hormone */
UNION
SELECT 'Hormone'::text  as type, medicine_administered.id as type_id, event_time,date_trunc('days',now()-event_time) as days_ago, (trade_name || ' at ' || location ||' by ' || medicine_administered.userid||'.') as text,null::text as extra1
FROM bovinemanagement.medicine_administered 
LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id = bovinecurr.id
WHERE medicine.medicine_class='hormone' AND bovine_id=func_bovine_id AND event_time is not null	
UNION        
/******************* Repro Hormone Scheduled */
SELECT 'Hormone Scheduled'::text  as type, medicine_administered.id as type_id, scheduled_event_time as event_time,date_trunc('days',now()-scheduled_event_time) as days_ago, concat(trade_name , ' at ' , location,'.') as text,null::text as extra1
FROM bovinemanagement.medicine_administered 
LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id = bovinecurr.id
WHERE medicine.medicine_class='hormone' AND bovine_id=func_bovine_id AND event_time is null        

ORDER BY event_time DESC;


END
$$;


--
-- Name: rfid_number_to_partial(numeric); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.rfid_number_to_partial(func_in numeric) RETURNS numeric
    LANGUAGE sql
    AS $$SELECT ((regexp_matches(func_in::text,	E'([0-9]{3}[0]*)([0-9]*)') )[2])::numeric$$;


--
-- Name: FUNCTION rfid_number_to_partial(func_in numeric); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.rfid_number_to_partial(func_in numeric) IS 'convers rfid number to one without country code and leading zeros, used to match with urban feeders format.';


--
-- Name: rfid_purebred(numeric); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.rfid_purebred(func_rfid numeric) RETURNS boolean
    LANGUAGE plpgsql
    AS $$/* this checks if rfid number is purebred, exception when out of range */


DECLARE 
answer boolean;
purebred2 boolean;
BEGIN

/* find out if the rfid number is valid and from purebred/registered tag set or not */

Select purebred  FROM bovinemanagement.eartag_valid where rfid_range @> func_rfid into purebred2;

IF (purebred2 = TRUE) THEN
answer = TRUE::boolean;
ELSEIF (purebred2 = FALSE) THEN
answer = FALSE::boolean;
ELSE 
RAISE EXCEPTION 'RFID % is not in a valid range', func_rfid
      USING HINT = 'Please check your rfid number is correct and if so add it to eartag_valid';
END IF;

return answer::boolean;

END;$$;


--
-- Name: FUNCTION rfid_purebred(func_rfid numeric); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.rfid_purebred(func_rfid numeric) IS 'checks if an rfid number is from a purebred tag set. Also throws error if it is no tag set ranges.';


--
-- Name: rfid_valid(numeric); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.rfid_valid(func_rfid numeric) RETURNS boolean
    LANGUAGE plpgsql IMMUTABLE
    AS $$/* this checks if rfid number is in a valid  range*/


DECLARE
answer1 numrange; 
answer boolean;
BEGIN

/* find out if the rfid number is valid and from purebred/registered tag set or not */

Select rfid_range FROM bovinemanagement.eartag_valid where func_rfid <@ rfid_range into answer1 limit 1;

IF (FOUND = TRUE) THEN
answer = TRUE::boolean;
ELSE
answer = FALSE::boolean;
END IF;

return answer::boolean;

END;$$;


--
-- Name: FUNCTION rfid_valid(func_rfid numeric); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.rfid_valid(func_rfid numeric) IS 'true if rfid is within valid range, false when it is not';


--
-- Name: round_to_nearest_date(timestamp without time zone); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.round_to_nearest_date(input_time timestamp without time zone) RETURNS date
    LANGUAGE plpgsql
    AS $$BEGIN
/* add one day */
IF date_part('hour', input_time)  >= 12
THEN
return  input_time::date + interval '1 day';
ELSE
/* leave it the same day*/
return  input_time::date;
END IF;

END;$$;


--
-- Name: semen_code_to_short_name(character); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.semen_code_to_short_name(func_semen_code character) RETURNS character varying
    LANGUAGE plpgsql
    AS $$/* Takes the semen code and returns the sire short name */
DECLARE
ret_short_name character varying;


BEGIN

SELECT into ret_short_name short_name FROM bovinemanagement.sire_semen_code 
left JOIN bovinemanagement.sire ON sire_semen_code.sire_full_reg_number = full_reg_number
where semen_code =func_semen_code; 

return ret_short_name;


END;$$;


--
-- Name: FUNCTION semen_code_to_short_name(func_semen_code character); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.semen_code_to_short_name(func_semen_code character) IS 'Takes the semen code and returns the sire short name';


--
-- Name: short_name(text); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.short_name(full_name text) RETURNS text
    LANGUAGE plpgsql
    AS $$BEGIN
/* turns a cows full name into the short name (last word of name) */
/* String length minus number of spaces plus one gives how many words in string. Then break up string and get just last word */
return split_part(full_name,' ',(length(full_name) + 1 - length(replace(full_name,' ',''))));
END;$$;


--
-- Name: FUNCTION short_name(full_name text); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.short_name(full_name text) IS ' turns a cows full name into the short name (last word of name).';


--
-- Name: stamp_update_log(); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.stamp_update_log() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN

BEGIN
      INSERT INTO system.update_log VALUES (TG_TABLE_SCHEMA,TG_TABLE_NAME);

EXCEPTION WHEN unique_violation THEN

      UPDATE system.update_log SET last_update='now()' WHERE schema_name=TG_TABLE_SCHEMA AND table_name=TG_TABLE_NAME;

END;

RETURN NEW;

END;

$$;


--
-- Name: FUNCTION stamp_update_log(); Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON FUNCTION bovinemanagement.stamp_update_log() IS 'a table calls this function via trigger whenever it is modified and it updates the timestamp log';


--
-- Name: update_time_column(); Type: FUNCTION; Schema: bovinemanagement; Owner: -
--

CREATE FUNCTION bovinemanagement.update_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
	BEGIN
	   NEW.update_time = now(); 
	   RETURN NEW;
	END;
	$$;


--
-- Name: base_saturation(integer); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.base_saturation(func_soil_sample_id integer) RETURNS cropping.base_saturation_result
    LANGUAGE plpgsql
    AS $$DECLARE 
       answer cropping.base_saturation_result;
       k numeric;
       ca numeric;
       mg numeric;
      na numeric;
      buffer_ph2 numeric;
     /* temp vars */
k_meq numeric;
 ca_meq numeric;
mg_meq numeric;
na_meq numeric;
base_Sum numeric;
acidty numeric;
/* output vars */
ca_base_sat numeric;
mg_base_sat numeric;
k_base_sat numeric;
na_base_sat numeric;
h_base_sat numeric;
total_base_sat numeric;
cec numeric;
BEGIN

SELECT potassium,calcium,magnesium,sodium,buffer_ph as x INTO k,ca,mg,na,buffer_ph2
FROM cropping.soil_sample_event WHERE soil_sample_event.id=func_soil_sample_id;

/* variables are loaded in, so do calculations */

/* atomic weight corrections */
        ca_meq:=ca / 200;
        mg_meq:=mg / 120;
        k_meq:=k  / 390;
        na_meq:=na / 230;
        base_Sum:= ca_meq + mg_meq + k_meq + na_meq;

/* correct for erroronous buffer ph values */
if (buffer_ph2 > 7) then
            acidty=0;
         else 
        acidty= 12*(7 - buffer_ph2);  /*formula from PEI lab */
END IF;

/* find percentages */
        cec:=round((base_Sum+acidty),1);
        ca_base_sat:=round((ca_meq/cec)*100,1);
        mg_base_sat:=round((mg_meq/cec)*100,1);
        k_base_sat:=round((k_meq/cec)*100,1);
        na_base_sat:=round((na_meq/cec)*100,1);
        h_base_sat:=round((acidty/cec)*100,1);
        total_base_sat:=round((ca_meq/cec + mg_meq/cec + k_meq/cec )*100,1);
      

/* load into answer*/
SELECT cec,
  ca_base_sat,
 mg_base_sat,
 k_base_sat,
 na_base_sat,
 h_base_sat,
 total_base_sat
INTO answer;

return answer;

END;$$;


--
-- Name: calculate_dm_linear_yield(numeric, numeric, text); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.calculate_dm_linear_yield(diameter_foot numeric, linear_length numeric, feed_type_name text) RETURNS numeric
    LANGUAGE plpgsql
    AS $$/* calculates dm yield based on bag diameter, number of liner feet, type of product. This assumes it doesn't change with moisture. Which might be correct. soybean silage must be more dense then grass, but no current info. */ 

DECLARE 
dm_kg_yield NUMERIC;
BEGIN

IF (feed_type_name = 'Corn Silage') THEN
/* 14 lb per cubic foot, 0.453592 changes to kg */ 
dm_kg_yield = linear_length*pi()*(diameter_foot/2)*(diameter_foot/2)*10*0.453592;
ELSEIF (feed_type_name = 'Corn Silage (BMR)') THEN
/* 14 lb per cubic foot, 0.453592 changes to kg */ 
dm_kg_yield = linear_length*pi()*(diameter_foot/2)*(diameter_foot/2)*10*0.453592;
ELSEIF (feed_type_name = 'Legume/Grass Mix') THEN
dm_kg_yield = linear_length*pi()*(diameter_foot/2)*(diameter_foot/2)*10*0.453592;
ELSEIF (feed_type_name = 'Native Grasses') THEN
dm_kg_yield = linear_length*pi()*(diameter_foot/2)*(diameter_foot/2)*10*0.453592;
ELSEIF (feed_type_name = 'Sorghum-Sudan Silage') THEN
dm_kg_yield = linear_length*pi()*(diameter_foot/2)*(diameter_foot/2)*10*0.453592;
ELSEIF (feed_type_name = 'Barley Silage') THEN
dm_kg_yield = linear_length*pi()*(diameter_foot/2)*(diameter_foot/2)*10*0.453592;
ELSEIF (feed_type_name = 'Haylage') THEN
dm_kg_yield = linear_length*pi()*(diameter_foot/2)*(diameter_foot/2)*10*0.453592;
ELSEIF (feed_type_name = 'Snaplage') THEN
/* uwex says 38 lb in http://www.uwex.edu/ces/dairynutrition/documents/cornsnaplagev42-2012.pdf */
dm_kg_yield = linear_length*pi()*(diameter_foot/2)*(diameter_foot/2)*38*0.453592;
ELSEIF (feed_type_name = 'Soybean Silage') THEN
/* this says soybean silage is more dense then corn silage http://ufdc.ufl.edu/UF00026448/00001/10j, but for tower silo, so just guess */
dm_kg_yield = linear_length*pi()*(diameter_foot/2)*(diameter_foot/2)*24*0.453592;
ELSE
RAISE EXCEPTION 'Feed type name:%, not supported for yield, please add and then retry', feed_type_name;

END IF;

RETURN dm_kg_yield;

END;$$;


--
-- Name: datum_area(integer); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.datum_area(func_datum_id integer) RETURNS numeric
    LANGUAGE plpgsql
    AS $$DECLARE
answer numeric;
BEGIN
SELECT INTO answer round((gis.ST_Area(gis.ST_Transform( geom,2036))/10000)::numeric,2) as area
FROM cropping.datum WHERE id=func_datum_id;
return answer;
END;$$;


--
-- Name: FUNCTION datum_area(func_datum_id integer); Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON FUNCTION cropping.datum_area(func_datum_id integer) IS 'provide datum id to get area in ha of datum';


--
-- Name: field_border_area_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.field_border_area_at_timestamp(function_field_id integer, func_event_time timestamp without time zone) RETURNS numeric
    LANGUAGE plpgsql
    AS $$DECLARE
answer numeric;
datum_id integer;
BEGIN

SELECT INTO datum_id border_event.datum_id FROM cropping.border_event WHERE field_id=function_field_id AND event_time <=func_event_time ORDER BY event_time LIMIT 1; 

SELECT INTO answer cropping.datum_area (datum_id);


RETURN answer;

END;$$;


--
-- Name: FUNCTION field_border_area_at_timestamp(function_field_id integer, func_event_time timestamp without time zone); Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON FUNCTION cropping.field_border_area_at_timestamp(function_field_id integer, func_event_time timestamp without time zone) IS 'Returns the area of a field in ha, for a given date';


--
-- Name: field_border_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.field_border_at_timestamp(function_field_id integer, func_event_time timestamp without time zone) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE
answer integer;
BEGIN

SELECT INTO answer datum_id FROM cropping.border_event WHERE field_id=function_field_id AND event_time <=func_event_time ORDER BY event_time LIMIT 1; 

RETURN answer;

END;$$;


--
-- Name: FUNCTION field_border_at_timestamp(function_field_id integer, func_event_time timestamp without time zone); Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON FUNCTION cropping.field_border_at_timestamp(function_field_id integer, func_event_time timestamp without time zone) IS 'finds what field border was active at date (timestamp) of interest';


--
-- Name: field_crop_yield_year(integer); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.field_crop_yield_year(func_year integer) RETURNS SETOF cropping.cutting_yield
    LANGUAGE sql
    AS $_$  With temp as(
SELECT event_time,field.id as field_id,alpha_numeric_id ,abs(footage_finish-footage_start) as footage_yield, diameter_foot, nutrition.bag_feed_name_at_footage(bag.id::integer, bag_field.footage_start::integer)::text as feed_type_name,
 ( SELECT border.datum_id
                   FROM cropping.border_event border
                  WHERE border.field_id = field.id AND border.event_time = (( SELECT max(border.event_time) AS max
                           FROM cropping.border_event border
                          WHERE border.field_id = field.id AND border.event_time <=  ($1  || '-12-31')::date ))) AS datum_id

 FROM nutrition.bag
LEFT JOIN nutrition.bag_field ON bag_id=bag.id 
LEFT JOIN cropping.field on field_id=field.id
WHERE event_time > ($1  || '-04-01')::date  AND event_time <= ($1  || '-12-31')::date 
), temp2 as(
SELECT *,sum(footage_yield) OVER (partition BY field_id, feed_type_name),geom as border_geom
 FROM temp
LEFT JOIN cropping.datum ON temp.datum_id=datum.id
), temp3 as (
SELECT DISTINCT(field_id,feed_type_name) event_time,field_id,alpha_numeric_id, diameter_foot,feed_type_name,sum as season_yield,gis.ST_Area(gis.ST_Transform(border_geom, 2036))/10000 as field_ha 
FROM temp2  WHERE field_id is not null ORDER BY alpha_numeric_id	
)
SELECT field_id,alpha_numeric_id,$1 as year,field_ha::numeric,feed_type_name,diameter_foot,season_yield  as yield_bag_feet, cropping.calculate_dm_linear_yield (diameter_foot, season_yield, feed_type_name) as linear_yield
FROM temp3 ORDER BY feed_type_name,alpha_numeric_id$_$;


--
-- Name: FUNCTION field_crop_yield_year(func_year integer); Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON FUNCTION cropping.field_crop_yield_year(func_year integer) IS 'Returns one year of bag yields. Groups by crop, ie 3 cuttings of alfalfa come out as one yield and snaplage and corn silage from same field come out as two different rows.';


--
-- Name: field_parameter_id_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.field_parameter_id_at_timestamp(func_field_id integer, func_event_time timestamp without time zone) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE 
output INTEGER;

/* gets the field paramater id of specific field at the date of interest */   
BEGIN

SELECT INTO output id FROM cropping.field_parameter WHERE event_time <= func_event_time AND field_id=func_field_id ORDER BY event_time DESC LIMIT 1;

return output;

END;$$;


--
-- Name: FUNCTION field_parameter_id_at_timestamp(func_field_id integer, func_event_time timestamp without time zone); Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON FUNCTION cropping.field_parameter_id_at_timestamp(func_field_id integer, func_event_time timestamp without time zone) IS 'used usually to find the latest field parameters for a field';


--
-- Name: field_short_name(integer); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.field_short_name(func_field_id integer) RETURNS character varying
    LANGUAGE sql
    AS $$select alpha_numeric_id	from cropping.field WHERE id=func_field_id$$;


--
-- Name: lime_event_id_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.lime_event_id_at_timestamp(func_field_id integer, func_event_time timestamp without time zone) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE 
output INTEGER;

BEGIN

SELECT INTO output id FROM cropping.lime_event WHERE event_time <= func_event_time AND field_id=func_field_id ORDER BY event_time DESC LIMIT 1;

return output;

END;$$;


--
-- Name: FUNCTION lime_event_id_at_timestamp(func_field_id integer, func_event_time timestamp without time zone); Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON FUNCTION cropping.lime_event_id_at_timestamp(func_field_id integer, func_event_time timestamp without time zone) IS 'usually used for latest lime events.';


--
-- Name: rotation_establishment_event(integer, integer); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.rotation_establishment_event(func_field_id integer, func_crop_year integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$/* takes field id and year to determine at any time when the start of the rotation was. */ 
/* Looks up current years crop and then goes back in time until it changes. */

DECLARE
latest_seeding_year integer;
seeding_id integer;
perennial boolean;
func_crop_date date := 	to_date(func_crop_year::text || '-12-31','YYYY-MM-DD'); /* create last day of the year date */
mviews RECORD;
counter INTEGER;
 latest_crop_seed_id INTEGER;
cursor_seed_event_id INTEGER;
BEGIN

counter =0;

FOR mviews IN 
 SELECT seed_event.seed_id, seed_event.id,seed_event.event_time, seed_category.general_type, seed_category.specific_type, seed_category.perennial, date_trunc('year'::text, age(date_trunc('year'::text, seed_event.event_time))) AS date_trunc
   FROM cropping.seed_event
   LEFT JOIN cropping.seed ON seed.id = seed_event.seed_id
   LEFT JOIN cropping.seed_category ON seed_category.id = seed.seed_category_id
  WHERE seed_event.field_id = func_field_id AND rotational_crop=true AND seed_event.event_time <= func_crop_date
  ORDER BY seed_event.event_time DESC


 
/*   start looping through results */
LOOP

/* init */
IF  (counter = 0) THEN
   latest_crop_seed_id=mviews.seed_id;
   cursor_seed_event_id=mviews.id;
END IF;

/* ?? */
IF (latest_crop_seed_id != mviews.seed_id) THEN
return cursor_seed_event_id;
ELSE
cursor_seed_event_id=mviews.id;
END IF;

counter = counter +1;

END LOOP;

return cursor_seed_event_id;


END;$$;


--
-- Name: soil_sample_event_id_at_timestamp(integer, timestamp without time zone); Type: FUNCTION; Schema: cropping; Owner: -
--

CREATE FUNCTION cropping.soil_sample_event_id_at_timestamp(func_field_id integer, func_event_time timestamp without time zone) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE 
output INTEGER;

BEGIN

SELECT INTO output id FROM cropping.soil_sample_event WHERE event_time <= func_event_time AND field_id=func_field_id ORDER BY event_time DESC LIMIT 1;

return output;

END;$$;


--
-- Name: FUNCTION soil_sample_event_id_at_timestamp(func_field_id integer, func_event_time timestamp without time zone); Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON FUNCTION cropping.soil_sample_event_id_at_timestamp(func_field_id integer, func_event_time timestamp without time zone) IS 'find the soil sample at time of interest, usually used to find latest one with now().';


--
-- Name: create_time_column(); Type: FUNCTION; Schema: hr; Owner: -
--

CREATE FUNCTION hr.create_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
	BEGIN
	   NEW.create_time = now(); 
	   RETURN NEW;
	END;
	$$;


--
-- Name: stamp_update_log(); Type: FUNCTION; Schema: hr; Owner: -
--

CREATE FUNCTION hr.stamp_update_log() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN

BEGIN
      INSERT INTO system.update_log VALUES (TG_TABLE_SCHEMA,TG_TABLE_NAME);

EXCEPTION WHEN unique_violation THEN

      UPDATE system.update_log SET last_update='now()' WHERE schema_name=TG_TABLE_SCHEMA AND table_name=TG_TABLE_NAME;

END;

RETURN NEW;

END;

$$;


--
-- Name: FUNCTION stamp_update_log(); Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON FUNCTION hr.stamp_update_log() IS 'a table calls this function via trigger whenever it is modified and it updates the timestamp log';


--
-- Name: update_time_column(); Type: FUNCTION; Schema: hr; Owner: -
--

CREATE FUNCTION hr.update_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN
	   NEW.update_time = now(); 
	   RETURN NEW;
	END;$$;


--
-- Name: avg_array(numeric[]); Type: FUNCTION; Schema: misc; Owner: -
--

CREATE FUNCTION misc.avg_array(numeric[]) RETURNS numeric
    LANGUAGE sql
    AS $_$SELECT avg(v) FROM unnest($1) g(v)
$_$;


--
-- Name: FUNCTION avg_array(numeric[]); Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON FUNCTION misc.avg_array(numeric[]) IS 'averages the values from an array.';


--
-- Name: generate_dates(date, date, integer); Type: FUNCTION; Schema: misc; Owner: -
--

CREATE FUNCTION misc.generate_dates(dt1 date, dt2 date, n integer) RETURNS SETOF date
    LANGUAGE sql IMMUTABLE
    AS $_$  SELECT $1 + i
  FROM generate_series(0, $2 - $1, $3) i;
$_$;


--
-- Name: FUNCTION generate_dates(dt1 date, dt2 date, n integer); Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON FUNCTION misc.generate_dates(dt1 date, dt2 date, n integer) IS 'generates a list of dates between start and end, with a skip value. normally generate_dates(''2011-11-11'',''2012-11-11'',1) is what you want';


--
-- Name: OLD_recipe_for_location_date(integer, timestamp without time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition."OLD_recipe_for_location_date"(func_location_id integer, func_date timestamp without time zone) RETURNS integer
    LANGUAGE plpgsql
    AS $$/* takes the location id and the date and recturns whatever recipe was active at that time, assumes the latest one was active. */

DECLARE
answer integer;
recp_date timestamp without time zone;
BEGIN

SELECT max(recipe.date) INTO recp_date FROM nutrition.mix 
LEFT JOIN nutrition.recipe on mix.recipe_id=recipe.id
WHERE location_id=func_location_id AND recipe.date <= func_date;

/* now that we know max time, return the answer */
SELECT recipe.id INTO answer FROM nutrition.mix 
LEFT JOIN nutrition.recipe on mix.recipe_id=recipe.id
WHERE location_id=func_location_id AND recipe.date = recp_date limit 1;

RETURN answer;

END;$$;


--
-- Name: FUNCTION "OLD_recipe_for_location_date"(func_location_id integer, func_date timestamp without time zone); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition."OLD_recipe_for_location_date"(func_location_id integer, func_date timestamp without time zone) IS 'returns the recipe for a specific location and time. Used for historical calculations.';


SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: feed_library_nrc2001; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.feed_library_nrc2001 (
    "Feed Name" text NOT NULL,
    "Category" text,
    "Read Only" boolean,
    "IFN" text,
    "TDN (%DM)" numeric,
    "Energy Eq Class" text,
    "Forage Description" text,
    "PAF" numeric,
    "DE (Mcal/kg)" numeric,
    "DM (%AF)" numeric,
    "NDF (%DM)" numeric,
    "ADF (%DM)" numeric,
    "Lignin (%DM)" numeric,
    "CP (%DM)" numeric,
    "NDFIP (%DM)" text,
    "ADFIP (%DM)" text,
    "Prt-A (%CP)" numeric,
    "Prt-B (%CP)" numeric,
    "Prt-C (%CP)" numeric,
    "Kd (1/hr)" numeric,
    "RUP Digest (%)" numeric,
    "Fat (%DM)" numeric,
    "Ash (%DM)" numeric,
    "Ca (%DM)" numeric,
    "Ca-Bio (g/g)" numeric,
    "P (%DM)" numeric,
    "P-Bio (g/g)" numeric,
    "Mg (%DM)" numeric,
    "Mg-Bio (g/g)" numeric,
    "K (%DM)" numeric,
    "K-Bio (g/g)" numeric,
    "Na (%DM)" numeric,
    "Na-Bio (g/g)" numeric,
    "Cl (%DM)" numeric,
    "Cl-Bio (g/g)" numeric,
    "S (%DM)" numeric,
    "S-Bio (g/g)" numeric,
    "Co (mg/kg)" numeric,
    "Co-Bio (g/g)" numeric,
    "Cu (mg/kg)" numeric,
    "Cu-Bio (g/g)" numeric,
    "I (mg/kg)" numeric,
    "I-Bio (g/g)" numeric,
    "Fe (mg/kg)" numeric,
    "Fe-Bio (g/g)" numeric,
    "Mn (mg/kg)" numeric,
    "Mn-Bio (g/g)" numeric,
    "Se (mg/kg)" numeric,
    "Se-Bio (g/g)" numeric,
    "Zn (mg/kg)" numeric,
    "Zn-Bio (g/g)" numeric,
    "Arg (%CP)" numeric,
    "His (%CP)" numeric,
    "Ile (%CP)" numeric,
    "Leu (%CP)" numeric,
    "Lys (%CP)" numeric,
    "Met (%CP)" numeric,
    "Cys (%CP)" numeric,
    "Phe (%CP)" numeric,
    "Thr (%CP)" numeric,
    "Trp (%CP)" numeric,
    "Val (%CP)" numeric,
    "Vit-A (1000 IU/kg)" numeric,
    "Vit-D (1000 IU/kg)" numeric,
    "Vit-E (IU/kg)" numeric,
    "NFC Digest" numeric,
    "CP Digest" numeric,
    "NDF Digest" numeric,
    "Fat Digest" numeric,
    "cDM (%)" numeric,
    "cGE (Mcal/kg)" numeric,
    "cDE (Mcal/kg)" numeric,
    "cME (Mcal/kg)" numeric,
    "cME/cGE" numeric,
    "cNEm (Mcal/kg)" numeric,
    "cNEg (Mcal/kg)" numeric,
    "cCP (%DM)" numeric,
    "cDCP (%DM)" numeric,
    "cEE (%DM)" numeric,
    "cASH (%DM)" numeric
);


--
-- Name: TABLE feed_library_nrc2001; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.feed_library_nrc2001 IS 'book values for all types of feeds from NRC 2001 (password "dairy cows")';


--
-- Name: bag_analysis_overlay_feed_library(); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.bag_analysis_overlay_feed_library() RETURNS SETOF nutrition.feed_library_nrc2001
    LANGUAGE plpgsql
    AS $$ 
DECLARE
  bagrow record;
  nrc nutrition.feed_library_nrc2001; /* set type of row to same as table */
BEGIN

create temp table output3 (like nutrition.feed_library_nrc2001) ON COMMIT DROP;

FOR bagrow IN
 SELECT bag_analysis.id,"bag_id","footage","sample_number","sample_date",	nutrition.bag_feed_name_at_footage(bag_id, footage) as bag_feed_name_at_footage ,nutrition.bag_field_id_at_footage(bag_id, footage),nutrition.bag_ensile_date_at_footage (bag_id, footage),
(nrc2001_template->>'Sodium')::numeric as "Sodium",
(nrc2001_template->>'Calcium')::numeric  as "Calcium",
(nrc2001_template->>'Sulphur')::numeric  as "Sulphur",
(nrc2001_template->>'CP (%DM)')::numeric  as "CP (%DM)",
(nrc2001_template->>'DM (%AF)')::numeric  as "DM (%AF)", 
(nrc2001_template->>'ADF (%DM)')::numeric  as "ADF (%DM)",
(nrc2001_template->>'Ash (%DM)')::numeric  as "Ash (%DM)",
(nrc2001_template->>'Fat (%DM)')::numeric  as "Fat (%DM)",
(nrc2001_template->>'Kd (1/hr)')::numeric  as "Kd (1/hr)",
(nrc2001_template->>'Magnesium')::numeric  as "Magnesium",
(nrc2001_template->>'NDF (%DM)')::numeric  as "NDF (%DM)",
(nrc2001_template->>'Potassium')::numeric  as "Potassium",
(nrc2001_template->>'TDN (%DM)')::numeric  as "TDN (%DM)",
(nrc2001_template->>'Phosphorus')::numeric  as "Phosphorus",
(nrc2001_template->>'ADFIP (%DM)')::numeric  as "ADFIP (%DM)",
(nrc2001_template->>'NDFIP (%DM)')::numeric  as "NDFIP (%DM)",
(nrc2001_template->>'Prt-A (%CP)')::numeric  as "Prt-A (%CP)",
(nrc2001_template->>'Prt-B (%CP)')::numeric  as "Prt-B (%CP)",
(nrc2001_template->>'Prt-C (%CP)')::numeric  as "Prt-C (%CP)",
(nrc2001_template->>'DE (Mcal/kg)')::numeric  as "DE (Mcal/kg)",
(nrc2001_template->>'Lignin (%DM)')::numeric  as "Lignin (%DM)",
(nrc2001_template->>'RUP Digest (%)')::numeric  as "RUP Digest (%)"
FROM nutrition.bag_analysis 
WHERE nutrition.bag_feed_name_at_footage(bag_id, footage) is not null AND nrc2001_template is not null
 LOOP

IF bagrow.bag_feed_name_at_footage = 'Corn Silage' THEN
    /* just overwrite the "corn silage, normal" values from NRC2001 */
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Corn Silage, normal' limit 1;
ELSEIF bagrow.bag_feed_name_at_footage = 'Corn Silage (BMR)' THEN
    /* just overwrite the "corn silage, normal" values from NRC200, we do not have a specific profile yet */
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Corn Silage, normal' limit 1;
ELSIF bagrow.bag_feed_name_at_footage = 'Legume/Grass Mix' THEN
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Mix Grass+Leg. Sil., midmat.' limit 1;
ELSIF bagrow.bag_feed_name_at_footage = 'Soybean Silage' THEN
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Soybean, Silage, early mat.' limit 1;
ELSIF bagrow.bag_feed_name_at_footage = 'Sorghum-Sudan Silage' THEN
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Sorghum, Sudan Type, Silage' limit 1;
ELSIF bagrow.bag_feed_name_at_footage = 'Snaplage' THEN
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Corn Grain+cob,grnd hi moist' limit 1;


END IF;

 

/* overwrite default nrc values with bag analysis values when not null */
nrc."Na (%DM)":= coalesce(bagrow."Sodium",nrc."Na (%DM)");
nrc."Ca (%DM)":= coalesce(bagrow."Calcium",nrc."Ca (%DM)");
nrc."S (%DM)":= coalesce(bagrow."Sulphur",nrc."S (%DM)");
nrc."CP (%DM)":= coalesce(bagrow."CP (%DM)",nrc."CP (%DM)");
nrc."DM (%AF)":= coalesce(bagrow."DM (%AF)",nrc."DM (%AF)");
nrc."ADF (%DM)":= coalesce(bagrow."ADF (%DM)",nrc."ADF (%DM)");
nrc."Ash (%DM)":= coalesce(bagrow."Ash (%DM)",nrc."Ash (%DM)");
nrc."Fat (%DM)":= coalesce(bagrow."Fat (%DM)",nrc."Fat (%DM)");
nrc."Kd (1/hr)":= coalesce(bagrow."Kd (1/hr)",nrc."Kd (1/hr)");
nrc."Mg (%DM)":= coalesce(bagrow."Magnesium",nrc."Mg (%DM)");
nrc."NDF (%DM)":= coalesce(bagrow."NDF (%DM)",nrc."NDF (%DM)");
nrc."K (%DM)":= coalesce(bagrow."Potassium",nrc."K (%DM)");
nrc."TDN (%DM)":= coalesce(bagrow."TDN (%DM)",nrc."TDN (%DM)");
nrc."P (%DM)":= coalesce(bagrow."Phosphorus",nrc."P (%DM)");
/* ADFIP and NDFIP not supported by shur gain */
nrc."Prt-A (%CP)":= coalesce(bagrow."Prt-A (%CP)",nrc."Prt-A (%CP)");
nrc."Prt-B (%CP)":= coalesce(bagrow."Prt-B (%CP)",nrc."Prt-B (%CP)");
nrc."Prt-C (%CP)":= coalesce(bagrow."Prt-C (%CP)",nrc."Prt-C (%CP)");
nrc."DE (Mcal/kg)":= coalesce(bagrow."DE (Mcal/kg)",nrc."DE (Mcal/kg)");
nrc."Lignin (%DM)":= coalesce(bagrow."Lignin (%DM)",nrc."Lignin (%DM)");
nrc."RUP Digest (%)":= coalesce(bagrow."RUP Digest (%)",nrc."RUP Digest (%)");
/* starch is ignored, nrc2001 uses fiber, which is inverse of starch, need more info to implement */



/* to save time, because it is a pain to modify the table and add a field, use the IFN field to stand in for the bag_analysis_id. */
nrc."IFN":=bagrow.id;
nrc."Feed Name":= concat_ws(' ',bagrow.bag_feed_name_at_footage::text,cropping.field_short_name(bagrow.bag_field_id_at_footage)::text ,' ft:',bagrow.footage, bagrow.bag_ensile_date_at_footage::text);	

insert into output3 select nrc.*;
 RAISE NOTICE 'Test --> %',nrc."NFC Digest";
   END LOOP;



  RETURN QUERY SELECT * FROM output3;
END;
$$;


--
-- Name: FUNCTION bag_analysis_overlay_feed_library(); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.bag_analysis_overlay_feed_library() IS 'overlays bag analysis data over the corresponding NRC 2001 feed library data.';


--
-- Name: bag_analysis_overlay_feed_library_bag_current(); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.bag_analysis_overlay_feed_library_bag_current() RETURNS SETOF nutrition.feed_library_nrc2001
    LANGUAGE plpgsql
    AS $$ 
DECLARE 
  bagrow record;
  nrc nutrition.feed_library_nrc2001; /* set type of row to same as table */
BEGIN

create temp table output3 (like nutrition.feed_library_nrc2001) ON COMMIT DROP;

FOR bagrow IN
SELECT bag_analysis.id,"bag_id","footage","sample_number","sample_date",	nutrition.bag_feed_name_at_footage(bag_id, footage) as bag_feed_name_at_footage ,nutrition.bag_field_id_at_footage(bag_id, footage),nutrition.bag_ensile_date_at_footage (bag_id, footage),
(nrc2001_template->>'Sodium')::numeric as "Sodium",
(nrc2001_template->>'Calcium')::numeric  as "Calcium",
(nrc2001_template->>'Sulphur')::numeric  as "Sulphur",
(nrc2001_template->>'CP (%DM)')::numeric  as "CP (%DM)",
(nrc2001_template->>'DM (%AF)')::numeric  as "DM (%AF)", 
(nrc2001_template->>'ADF (%DM)')::numeric  as "ADF (%DM)",
(nrc2001_template->>'Ash (%DM)')::numeric  as "Ash (%DM)",
(nrc2001_template->>'Fat (%DM)')::numeric  as "Fat (%DM)",
(nrc2001_template->>'Kd (1/hr)')::numeric  as "Kd (1/hr)",
(nrc2001_template->>'Magnesium')::numeric  as "Magnesium",
(nrc2001_template->>'NDF (%DM)')::numeric  as "NDF (%DM)",
(nrc2001_template->>'Potassium')::numeric  as "Potassium",
(nrc2001_template->>'TDN (%DM)')::numeric  as "TDN (%DM)",
(nrc2001_template->>'Phosphorus')::numeric  as "Phosphorus",
(nrc2001_template->>'ADFIP (%DM)')::numeric  as "ADFIP (%DM)",
(nrc2001_template->>'NDFIP (%DM)')::numeric  as "NDFIP (%DM)",
(nrc2001_template->>'Prt-A (%CP)')::numeric  as "Prt-A (%CP)",
(nrc2001_template->>'Prt-B (%CP)')::numeric  as "Prt-B (%CP)",
(nrc2001_template->>'Prt-C (%CP)')::numeric  as "Prt-C (%CP)",
(nrc2001_template->>'DE (Mcal/kg)')::numeric  as "DE (Mcal/kg)",
(nrc2001_template->>'Lignin (%DM)')::numeric  as "Lignin (%DM)",
(nrc2001_template->>'RUP Digest (%)')::numeric  as "RUP Digest (%)"
FROM nutrition.bag_analysis 
LEFT JOIN nutrition.bag_current ON bag_current.id=bag_analysis.bag_id
WHERE nutrition.bag_feed_name_at_footage (bag_id, footage) is not null AND bag_current.id is not null
 LOOP

IF bagrow.bag_feed_name_at_footage = 'Corn Silage' THEN
    /* just overwrite the "corn silage, normal" values from NRC2001 */
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Corn Silage, normal' limit 1;
ELSEIF bagrow.bag_feed_name_at_footage = 'Corn Silage (BMR)' THEN
    /* just overwrite the "corn silage, normal" values from NRC200, we do not have a specific profile yet */
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Corn Silage, normal' limit 1;
ELSIF bagrow.bag_feed_name_at_footage = 'Legume/Grass Mix' THEN
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Mix Grass+Leg. Sil., midmat.' limit 1;
ELSIF bagrow.bag_feed_name_at_footage = 'Soybean Silage' THEN
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Soybean, Silage, early mat.' limit 1;
ELSIF bagrow.bag_feed_name_at_footage = 'Sorghum-Sudan Silage' THEN
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Sorghum, Sudan Type, Silage' limit 1;
ELSIF bagrow.bag_feed_name_at_footage = 'Snaplage' THEN
SELECT INTO nrc *,null as "bag_analysis_id" FROM nutrition.feed_library_nrc2001 WHERE "Feed Name" = 'Corn Grain+cob,grnd hi moist' limit 1;

END IF;

 


/* overwrite default nrc values with bag analysis values when not null */
nrc."Na (%DM)":= coalesce(bagrow."Sodium",nrc."Na (%DM)");
nrc."Ca (%DM)":= coalesce(bagrow."Calcium",nrc."Ca (%DM)");
nrc."S (%DM)":= coalesce(bagrow."Sulphur",nrc."S (%DM)");
nrc."CP (%DM)":= coalesce(bagrow."CP (%DM)",nrc."CP (%DM)");
nrc."DM (%AF)":= coalesce(bagrow."DM (%AF)",nrc."DM (%AF)");
nrc."ADF (%DM)":= coalesce(bagrow."ADF (%DM)",nrc."ADF (%DM)");
nrc."Ash (%DM)":= coalesce(bagrow."Ash (%DM)",nrc."Ash (%DM)");
nrc."Fat (%DM)":= coalesce(bagrow."Fat (%DM)",nrc."Fat (%DM)");
nrc."Kd (1/hr)":= coalesce(bagrow."Kd (1/hr)",nrc."Kd (1/hr)");
nrc."Mg (%DM)":= coalesce(bagrow."Magnesium",nrc."Mg (%DM)");
nrc."NDF (%DM)":= coalesce(bagrow."NDF (%DM)",nrc."NDF (%DM)");
nrc."K (%DM)":= coalesce(bagrow."Potassium",nrc."K (%DM)");
nrc."TDN (%DM)":= coalesce(bagrow."TDN (%DM)",nrc."TDN (%DM)");
nrc."P (%DM)":= coalesce(bagrow."Phosphorus",nrc."P (%DM)");
/* ADFIP and NDFIP not supported by shur gain */
nrc."Prt-A (%CP)":= coalesce(bagrow."Prt-A (%CP)",nrc."Prt-A (%CP)");
nrc."Prt-B (%CP)":= coalesce(bagrow."Prt-B (%CP)",nrc."Prt-B (%CP)");
nrc."Prt-C (%CP)":= coalesce(bagrow."Prt-C (%CP)",nrc."Prt-C (%CP)");
nrc."DE (Mcal/kg)":= coalesce(bagrow."DE (Mcal/kg)",nrc."DE (Mcal/kg)");
nrc."Lignin (%DM)":= coalesce(bagrow."Lignin (%DM)",nrc."Lignin (%DM)");
nrc."RUP Digest (%)":= coalesce(bagrow."RUP Digest (%)",nrc."RUP Digest (%)");
/* starch is ignored, nrc2001 uses fiber, which is inverse of starch, need more info to implement */

/* to save time, because it is a pain to modify the table and add a field, use the IFN field to stand in for the bag_analysis_id. */
nrc."IFN":=bagrow.id;
nrc."Feed Name":= concat_ws(' ',bagrow.bag_feed_name_at_footage::text,cropping.field_short_name(bagrow.bag_field_id_at_footage)::text ,' ft:',bagrow.footage, bagrow.bag_ensile_date_at_footage::text);	

insert into output3 select nrc.*;
 RAISE NOTICE 'Test --> %',nrc."NFC Digest";
   END LOOP;

  RETURN QUERY SELECT * FROM output3;
END;
$$;


--
-- Name: bag_closest_analysis_id(integer, integer, text); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.bag_closest_analysis_id(func_bag_id integer, func_bag_footage integer, func_bag_direction text) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE
answer integer;
temp_footage integer;
BEGIN

/* assume analysis could be taken at any time, so just get the closest DM to the requested footage, but returns ID instead. */ 
/* This doesn't check if they feed type changed in the bag which would be highly correlated with a analysis change */
/* When direction is in forward or reverse IT ALWAYS TAKES THE CLOSEST TO THE SPECIFIED FOOTAGE */


 IF func_bag_direction = 'forward' 
   THEN
 SELECT INTO temp_footage footage,min(abs(footage-func_bag_footage)) FROM nutrition.bag_analysis WHERE bag_id=func_bag_id  group by footage order by min asc  limit 1;
   ELSEIF func_bag_direction = 'reverse'
   THEN
    SELECT INTO temp_footage footage,min(abs(footage-func_bag_footage)) FROM nutrition.bag_analysis WHERE bag_id=func_bag_id  group by footage order by min asc  limit 1;
 ELSE
   answer:=null;
  RETURN answer;
END IF;

SELECT INTO answer id FROM nutrition.bag_analysis WHERE bag_id=func_bag_id AND footage=temp_footage;


RETURN answer;

END;


$$;


--
-- Name: FUNCTION bag_closest_analysis_id(func_bag_id integer, func_bag_footage integer, func_bag_direction text); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.bag_closest_analysis_id(func_bag_id integer, func_bag_footage integer, func_bag_direction text) IS 'returns bag analysis id for closest footage and direction specified';


--
-- Name: bag_closest_dry_matter_id(integer, integer, text); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.bag_closest_dry_matter_id(func_bag_id integer, func_bag_footage integer, func_bag_direction text) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE
answer integer;
temp_footage integer;
BEGIN

/* assume moisture test could be taken at any time, so just get the closest DM to the requested footage, but returns ID instead. */ 
/* This doesn't check if they feed type changed in the bag which would be highly correlated with a moisture change */
/* When direction is in forward direction it takes the moisture behind the lead edge as preferred, opposite for reverse */


 IF func_bag_direction = 'forward' 
   THEN
 SELECT INTO temp_footage footage,min(abs(footage-func_bag_footage)) FROM nutrition.bag_moisture_test WHERE bag_id=func_bag_id AND footage <= func_bag_footage group by footage order by min asc  limit 1;
   ELSEIF func_bag_direction = 'reverse'
   THEN
    SELECT INTO temp_footage footage,min(abs(footage-func_bag_footage)) FROM nutrition.bag_moisture_test WHERE bag_id=func_bag_id AND footage >= func_bag_footage group by footage order by min asc  limit 1;
 ELSE
   answer:=null;
  RETURN answer;
END IF;

SELECT INTO answer id FROM nutrition.bag_moisture_test WHERE bag_id=func_bag_id AND footage=temp_footage;


RETURN answer;

END;$$;


--
-- Name: bag_current_consumption_footage(integer, text); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.bag_current_consumption_footage(func_bag_id integer, func_bag_direction text) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE
answer text;
BEGIN

 IF func_bag_direction = 'forward' 
   THEN
      SELECT INTO answer max(footage) FROM nutrition.bag_consumption WHERE bag_id=func_bag_id AND direction='forward'  LIMIT 1;
   ELSEIF func_bag_direction = 'reverse'
   THEN
      SELECT INTO answer min(footage) FROM nutrition.bag_consumption WHERE bag_id=func_bag_id AND direction='reverse' LIMIT 1;
 ELSE
   answer:=null;
END IF;

RETURN answer;

END;$$;


--
-- Name: bag_dimenension(integer); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.bag_dimenension(func_bag_id integer) RETURNS numeric[]
    LANGUAGE sql
    AS $$SELECT array[min(footage_start), max(footage_finish)] FROM nutrition.bag_feed WHERE bag_id=func_bag_id$$;


--
-- Name: FUNCTION bag_dimenension(func_bag_id integer); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.bag_dimenension(func_bag_id integer) IS 'returns numeric array of min footage and max footage over all feed types, thus giving actual dimensions of the bag';


--
-- Name: bag_ensile_date_at_footage(integer, numeric); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.bag_ensile_date_at_footage(func_bag_id integer, func_footage numeric) RETURNS date
    LANGUAGE plpgsql
    AS $$DECLARE
answer date;
dummy numeric;
BEGIN


/* first try and do the closest footage*/
SELECT INTO answer,dummy  event_time,(abs(footage-func_footage)) as closest_footage FROM nutrition.bag_ensile_date WHERE bag_id=func_bag_id ORDER BY closest_footage limit 1;

/* when all else fails, just use the date from when bag was started */
IF answer is null 
THEN
SELECT INTO answer event_time FROM nutrition.bag WHERE  id=func_bag_id;
END IF;

RETURN answer;

END;$$;


--
-- Name: FUNCTION bag_ensile_date_at_footage(func_bag_id integer, func_footage numeric); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.bag_ensile_date_at_footage(func_bag_id integer, func_footage numeric) IS 'returns the closest date to when the field was ensiled. this is used mostly for historical reasons to find out when a field was harvested.';


--
-- Name: bag_feed_name_at_footage(integer, integer); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.bag_feed_name_at_footage(func_bag_id integer, func_footage integer) RETURNS text
    LANGUAGE plpgsql
    AS $$DECLARE
answer text;
BEGIN

SELECT feed_type.name FROM nutrition.bag 
LEFT JOIN nutrition.bag_feed ON bag.id=bag_id
LEFT JOIN nutrition.feed_type ON feed_id=feed_type.id
WHERE bag.id=func_bag_id
AND footage_start<=func_footage AND footage_finish>=func_footage LIMIT 1 INTO answer;

RETURN answer;

END;$$;


--
-- Name: bag_field_id_at_footage(integer, numeric); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.bag_field_id_at_footage(func_bag_id integer, func_footage numeric) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE
answer integer;
BEGIN


/* searches numeric range to see if footage is contained. Should only ever return one row unless there is a field overlap. Returns null if can't find anyhting */ 
SELECT INTO answer field_id FROM nutrition.bag_field WHERE bag_id=func_bag_id AND numrange(footage_start,footage_finish) @> func_footage::numeric;




RETURN answer;

END;$$;


--
-- Name: FUNCTION bag_field_id_at_footage(func_bag_id integer, func_footage numeric); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.bag_field_id_at_footage(func_bag_id integer, func_footage numeric) IS 'returns the closest field id to where the footage and bag_id specified.';


--
-- Name: create_time_column(); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.create_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN
	   NEW.create_time = now(); 
	   RETURN NEW;
	END;$$;


--
-- Name: day_ingredient_usage(date); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.day_ingredient_usage(func_date date) RETURNS TABLE(feedcurr_id text, date timestamp without time zone, dry_matter_percent numeric, day_ingredient_weighted_count numeric, day_ingredient_wet_amount numeric, day_ingredient_dry_amount numeric, feed_type_id integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY
   with temp as (
SELECT id, nutrition.recipe_for_location_date(location.id,func_date::timestamp)
FROM bovinemanagement.location
)
, temp2 as (
SELECT Distinct(recipe_for_location_date) as recipe_id ,func_date::timestamp as date  FROM temp WHERE recipe_for_location_date IS NOT NULL
), temp3 as (
SELECT temp2.date,temp2.recipe_id,(nutrition.mix_recipe_item_info(temp2.recipe_id,recipe_item.id)).*,recipe_item.feedcurr_id
FROM temp2
LEFT JOIN nutrition.recipe_item ON recipe_item.recipe_id = temp2.recipe_id
)
SELECT Distinct(temp3.feedcurr_id::text),temp3.date,temp3.dry_matter_percent,(sum(recipe_weighted_count) OVER (PARTITION BY temp3.feedcurr_id)) as day_ingredient_weighted_count     ,(sum(recipe_ingredient_wet_amount) OVER (PARTITION BY temp3.feedcurr_id)) as  day_ingredient_wet_amount   ,(sum(recipe_ingredient_dry_amount)  OVER (PARTITION BY temp3.feedcurr_id)) as   day_ingredient_dry_amount, (nutrition.ingredient_type (temp3.feedcurr_id, temp3.date::timestamp)).feed_type_id as feed_type_id

FROM temp3;
END
$$;


--
-- Name: ingredient_comment(text, timestamp without time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.ingredient_comment(feedcurr_id text, date_of_interest timestamp without time zone) RETURNS text
    LANGUAGE plpgsql
    AS $$DECLARE
/* assume feedcurr_id and date_of_interest are defined */

answer text;
footage_temp numeric;
var_table_name text;
var_table_id integer;
breakout nutrition.ingredient_feedcurr_breakout_result;

BEGIN
breakout := nutrition.ingredient_feedcurr_breakout(feedcurr_id);
var_table_name:=  breakout.table_name;
var_table_id:=  breakout.table_id;

 IF var_table_name = 'feed' 
   THEN
       SELECT INTO answer comment FROM nutrition.feed LEFT JOIN nutrition.feed_type ON feed.feed_type_id=feed_type.id  WHERE feed.id=var_table_id AND event_time <= date_of_interest ORDER BY event_time DESC LIMIT 1;
   ELSEIF (var_table_name = 'bag_f' OR  var_table_name = 'bag_r' )
   THEN
       SELECT INTO footage_temp footage FROM nutrition.bag_consumption WHERE bag_id= var_table_id AND event_time <= date_of_interest ORDER BY event_time DESC LIMIT 1;
/* footage is a single point for a comment, so allow a range, by looking +/- 10'. */
       SELECT INTO answer comment FROM nutrition.bag_comment WHERE bag_id = var_table_id AND (footage - 10)  <= footage_temp AND (footage + 10) >= footage_temp ORDER BY update_time DESC LIMIT 1;   
 ELSE
   RAISE EXCEPTION 'ingredient_comment : Unknown table name=%',var_table_name;
END IF;

RETURN answer;

END;
$$;


--
-- Name: ingredient_cost(text, timestamp without time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.ingredient_cost(feedcurr_id text, date_of_interest timestamp without time zone) RETURNS numeric
    LANGUAGE plpgsql
    AS $$  
 DECLARE
/* assume feedcurr_id and date_of_interest are defined */

answer numeric;
footage_temp numeric;
var_table_name text;
var_table_id integer;
breakout nutrition.ingredient_feedcurr_breakout_result;

BEGIN
breakout := nutrition.ingredient_feedcurr_breakout(feedcurr_id);
var_table_name:=  breakout.table_name;
var_table_id:=  breakout.table_id;

 IF var_table_name = 'feed' 
   THEN
       SELECT INTO answer cost FROM nutrition.feed_cost WHERE feed_id=var_table_id AND event_time <= date_of_interest ORDER BY event_time DESC LIMIT 1;
 ELSEIF  (var_table_name = 'bag_f' OR  var_table_name = 'bag_r' )
   THEN
      /* cast to date so that we change default consumption of 12 noon to 00:00. */
       SELECT INTO footage_temp footage FROM nutrition.bag_consumption WHERE bag_id= var_table_id AND event_time::date <= date_of_interest ORDER BY event_time::date DESC LIMIT 1;
       SELECT INTO answer cost FROM nutrition.bag_cost WHERE bag_id = var_table_id AND footage_start <= footage_temp AND footage_finish >= footage_temp;
 ELSE
   RAISE EXCEPTION 'ingredient_cost Unknown table name=%',var_table_name;
END IF;

RETURN answer;

END;
$$;


--
-- Name: ingredient_feedcurr_breakout(text); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.ingredient_feedcurr_breakout(feedcurr_id text) RETURNS nutrition.ingredient_feedcurr_breakout_result
    LANGUAGE plpgsql
    AS $$    
/* takes a string and breaks it into table and id parts. It would be much easier to have one uuid. */

DECLARE
answer nutrition.ingredient_feedcurr_breakout_result;
table_name text;
table_id integer;


BEGIN
/* fill the two vairbles by parsing the string */
SELECT INTO table_name substring(feedcurr_id from 0 for (position('|' in feedcurr_id)));
SELECT INTO table_id substring(feedcurr_id from (position('|' in feedcurr_id))+1)::integer;

answer:= (table_name,table_id) ;
return answer;

END;
$$;


--
-- Name: ingredient_footage(text, timestamp without time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.ingredient_footage(feedcurr_id text, date_of_interest timestamp without time zone) RETURNS numeric
    LANGUAGE plpgsql
    AS $$DECLARE
/* assume feedcurr_id and date_of_interest are defined */

answer text;
footage_temp numeric;
var_table_name text;
var_table_id integer;
breakout nutrition.ingredient_feedcurr_breakout_result;
var_clip_date timestamp;

BEGIN
breakout := nutrition.ingredient_feedcurr_breakout(feedcurr_id);
var_table_name:=  breakout.table_name;
var_table_id:=  breakout.table_id;
var_clip_date=date_of_interest::date + interval '12 hours';

/* only matches exact consumptions on the bag */

 IF var_table_name = 'feed' 
   THEN
       SELECT INTO answer null; /* there are no footages with feeds, only applies to bags */
   ELSEIF  (var_table_name = 'bag_f')
   THEN
       /* bag is going in a forward direction. Find the closest consumption to that date.*/
/* consumptions only recorded at 12 noon for the day */
 SELECT INTO answer footage from nutrition.bag_consumption WHERE bag_id=var_table_id AND event_time = var_clip_date AND direction='forward' LIMIT 1;
ELSEIF  (var_table_name = 'bag_r' ) 
   THEN
   SELECT INTO answer  footage from nutrition.bag_consumption WHERE bag_id=var_table_id AND event_time = var_clip_date AND direction='reverse' LIMIT 1;
ELSE
   RAISE EXCEPTION 'ingredient_footage: Unknown bag or feed name=%',var_table_name;
END IF;

RETURN answer;

END;$$;


--
-- Name: FUNCTION ingredient_footage(feedcurr_id text, date_of_interest timestamp without time zone); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.ingredient_footage(feedcurr_id text, date_of_interest timestamp without time zone) IS 'Returns the footage of a bag from a specified date. Not that useful. Used for historical calculations.';


--
-- Name: ingredient_info; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.ingredient_info (
    feedcurr_id text NOT NULL,
    table_name text NOT NULL,
    table_id integer NOT NULL,
    date timestamp without time zone NOT NULL,
    type text NOT NULL,
    location text NOT NULL,
    comment text,
    dry_matter_percent numeric NOT NULL,
    cost numeric NOT NULL,
    feed_type_id integer NOT NULL
);


--
-- Name: TABLE ingredient_info; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.ingredient_info IS 'holder table';


--
-- Name: ingredient_info(text, timestamp without time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.ingredient_info(feedcurr_id text, date_of_interest timestamp without time zone) RETURNS SETOF nutrition.ingredient_info
    LANGUAGE plpgsql
    AS $$  
DECLARE
/* assume feedcurr_id and date_of_interest are defined */
answer nutrition.ingredient_info; 
var_table_name text;
var_table_id integer;
breakout nutrition.ingredient_feedcurr_breakout_result;

BEGIN
breakout := nutrition.ingredient_feedcurr_breakout(feedcurr_id);
var_table_name:=  breakout.table_name;
var_table_id:=  breakout.table_id;

/*
answer.feedcurr_id := feedcurr_id;
answer.dry_matter_percent := nutrition.ingredient_moisture(feedcurr_id, date_of_interest);
answer.cost := nutrition.ingredient_cost(feedcurr_id, date_of_interest); 	
*/
/*SELECT feedcurr_id,nutrition.ingredient_moisture(feedcurr_id, date_of_interest),nutrition.ingredient_cost(feedcurr_id, date_of_interest) INTO answer;*/
/*SELECT 1,2,3 INTO answer;*/

/*RETURN answer;*/
SELECT feedcurr_id, var_table_name, var_table_id, date_of_interest,(nutrition.ingredient_type(feedcurr_id, date_of_interest)).name, nutrition.ingredient_location(feedcurr_id, date_of_interest), nutrition.ingredient_comment(feedcurr_id, date_of_interest), nutrition.ingredient_moisture(feedcurr_id, date_of_interest), nutrition.ingredient_cost(feedcurr_id, date_of_interest),(nutrition.ingredient_type(feedcurr_id, date_of_interest)).feed_type_id INTO answer;
return next answer;

END;

$$;


--
-- Name: FUNCTION ingredient_info(feedcurr_id text, date_of_interest timestamp without time zone); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.ingredient_info(feedcurr_id text, date_of_interest timestamp without time zone) IS 'main function to call';


--
-- Name: ingredient_location(text, timestamp without time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.ingredient_location(feedcurr_id text, date_of_interest timestamp without time zone) RETURNS text
    LANGUAGE plpgsql
    AS $$DECLARE
/* assume feedcurr_id and date_of_interest are defined */

answer text;
footage_temp numeric;
var_table_name text;
var_table_id integer;
breakout nutrition.ingredient_feedcurr_breakout_result;

BEGIN
breakout := nutrition.ingredient_feedcurr_breakout(feedcurr_id);
var_table_name:=  breakout.table_name;
var_table_id:=  breakout.table_id;

 IF var_table_name = 'feed' 
   THEN
       SELECT INTO answer location FROM nutrition.feed LEFT JOIN nutrition.feed_location ON feed.feed_location_id=feed_location.id  WHERE feed.id=var_table_id AND event_time <= date_of_interest ORDER BY event_time DESC LIMIT 1;
   ELSEIF  (var_table_name = 'bag_f' OR  var_table_name = 'bag_r' )
   THEN
       /* location is putting together two fields: location and slot */
       SELECT INTO answer (location || ' - slot ' || slot) FROM nutrition.bag WHERE id = var_table_id  ORDER BY event_time DESC LIMIT 1;   
 ELSE
   RAISE EXCEPTION 'ingredient_location: Unknown table name=%',var_table_name;
END IF;

RETURN answer;

END;$$;


--
-- Name: ingredient_moisture(text, timestamp without time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.ingredient_moisture(feedcurr_id text, date_of_interest timestamp without time zone) RETURNS numeric
    LANGUAGE plpgsql
    AS $$  
 DECLARE
/* assume feedcurr_id and date_of_interest are defined */

answer numeric;
footage_temp numeric;
var_table_name text;
var_table_id integer;
breakout nutrition.ingredient_feedcurr_breakout_result;

BEGIN
breakout := nutrition.ingredient_feedcurr_breakout(feedcurr_id);
var_table_name:=  breakout.table_name;
var_table_id:=  breakout.table_id;

 IF var_table_name = 'feed' 
   THEN
       SELECT INTO answer dry_matter_percent FROM nutrition.feed_moisture WHERE feed_id=var_table_id AND date <= date_of_interest ORDER BY date DESC LIMIT 1;
 ELSEIF  (var_table_name = 'bag_f' OR  var_table_name = 'bag_r' )
   THEN
       IF var_table_name = 'bag_f'
      THEN 
           SELECT INTO footage_temp footage FROM nutrition.bag_consumption WHERE bag_id= var_table_id AND event_time::date <= date_of_interest AND direction='forward' ORDER BY event_time DESC LIMIT 1;
      ELSE
         SELECT INTO footage_temp footage FROM nutrition.bag_consumption WHERE bag_id= var_table_id AND event_time::date <= date_of_interest AND direction='reverse' ORDER BY event_time DESC LIMIT 1;
     END IF;
       /* find moisture test closest to desired footage (minimizes based on the footage) */
       SELECT INTO answer percent_dry_matter FROM (SELECT  percent_dry_matter,min(abs(footage-footage_temp)) as min_footage_diff  FROM nutrition.bag_moisture_test WHERE bag_id = var_table_id GROUP BY percent_dry_matter ORDER BY min_footage_diff) as x ORDER BY min_footage_diff ASC LIMIT 1;
 ELSE
   RAISE EXCEPTION 'ingredient_moisture: Unknown table name=%',var_table_name;
END IF;

RETURN answer;

END;$$;


--
-- Name: ingredient_type(text, timestamp with time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.ingredient_type(feedcurr_id text, date_of_interest timestamp with time zone) RETURNS nutrition.ingredient_type_result
    LANGUAGE plpgsql
    AS $$DECLARE
/* assume feedcurr_id and date_of_interest are defined */

answer nutrition.ingredient_type_result;
out_name text;
out_id integer; 
footage_temp numeric;
var_table_name text;
var_table_id integer;
breakout nutrition.ingredient_feedcurr_breakout_result;

BEGIN
breakout := nutrition.ingredient_feedcurr_breakout(feedcurr_id);
var_table_name:=  breakout.table_name;
var_table_id:=  breakout.table_id;

 IF var_table_name = 'feed' 
   THEN
       SELECT INTO out_name,out_id name,feed_type_id FROM nutrition.feed LEFT JOIN nutrition.feed_type ON feed.feed_type_id=feed_type.id  WHERE feed.id=var_table_id AND event_time::date <= date_of_interest ORDER BY event_time DESC LIMIT 1;
   ELSEIF (var_table_name = 'bag_f' OR  var_table_name = 'bag_r' )
   THEN
    /* forward or reverse */
     IF (var_table_name = 'bag_f')
 THEN
         SELECT INTO footage_temp footage FROM nutrition.bag_consumption WHERE bag_id= var_table_id AND event_time::date <=date_of_interest AND direction='forward' ORDER BY event_time DESC LIMIT 1;

ELSE
       SELECT INTO footage_temp footage FROM nutrition.bag_consumption WHERE bag_id= var_table_id AND event_time::date <= date_of_interest AND direction='reverse' ORDER BY event_time DESC LIMIT 1;
   END IF;
/*  END forward or reverse */
    SELECT INTO out_name,out_id name,feed_type.id FROM nutrition.bag_feed LEFT JOIN nutrition.feed_type ON bag_feed.feed_id=feed_type.id WHERE bag_id = var_table_id AND footage_start <= footage_temp AND footage_finish >= footage_temp ORDER BY update_time DESC LIMIT 1;   
 ELSE
   RAISE EXCEPTION 'ingredient_type: Unknown table name=%,feedcurr_id=%',var_table_name, feedcurr_id;
END IF;

SELECT out_id,out_name INTO  answer;


RETURN answer;

END;$$;


--
-- Name: mix_cost(integer); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.mix_cost(func_mix_id integer) RETURNS numeric
    LANGUAGE plpgsql
    AS $$DECLARE
answer numeric;
BEGIN

/* assumes the recipe date as the date to calculate cost for */

WITH  temp_table as (
SELECT recipe.date as recipe_date FROM nutrition.mix
LEFT JOIN nutrition.recipe ON recipe.id = mix.recipe_id
WHERE mix.id=func_mix_id
), temp_table2 as (
SELECT recipe_date,nutrition.mix_info(func_mix_id,temp_table.recipe_date) FROM temp_table
)
SELECT  (mix_info).mix_cost_per_cow  FROM temp_table2 INTO answer;

RETURN answer;

END;
$$;


--
-- Name: FUNCTION mix_cost(func_mix_id integer); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.mix_cost(func_mix_id integer) IS 'takes any mix id and gives you the cost of the mix, taking into account the modification factor.';


--
-- Name: mix_info(integer, timestamp without time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.mix_info(func_mix_id integer, func_timestamp_of_interest timestamp without time zone) RETURNS SETOF nutrition.mix_info_result
    LANGUAGE plpgsql
    AS $$DECLARE 
answer nutrition.mix_info_result;
var_recipe_date timestamp without time zone;
var_next_recipe_date timestamp without time zone;


       mix_id integer;
       recipe_id integer; 
       recipe_start_timestamp timestamp without time zone;
       recipe_stop_timestamp timestamp without time zone;
       timestamp_of_interest timestamp without time zone;
       local_location_id integer;
       location_name text;
       location_count integer;
       mix_cost_per_cow numeric;
       mix_kg_day_dry_per_cow numeric;
       mix_as_fed_kg_per_cow numeric;
       mix_moisture_percent numeric;
       mix_modify numeric;
       total_mix_as_fed_kg numeric;


BEGIN

/*check if the func_timestamp_of_interest is in a valid date range. */
SELECT  recipe.date  FROM nutrition.mix 
LEFT JOIN nutrition.recipe ON recipe.id=mix.recipe_id
WHERE mix.id=func_mix_id INTO var_recipe_date;
IF var_recipe_date > func_timestamp_of_interest
THEN
RAISE EXCEPTION 'function mix_info (1): Input timestamp is before the recipe in mix existed: %.',var_recipe_date;
END IF;

/*check if it is a future date, only useful with the latest recipes */
IF localtimestamp < func_timestamp_of_interest
THEN
RAISE EXCEPTION 'Input timestamp is in the future: %.',func_timestamp_of_interest;
END IF;




timestamp_of_interest:=func_timestamp_of_interest;

Select mix.id,recipe.id,recipe.date,location.name,
(SELECT location_total_at_timestamp FROM bovinemanagement.location_total_at_timestamp(mix.location_id,func_timestamp_of_interest)),
(SELECT modification_factor FROM nutrition.mix_modify WHERE mix_modify.mix_id=mix.id AND date >= recipe.date ORDER BY date DESC LIMIT 1),mix.location_id
INTO mix_id,recipe_id,recipe_start_timestamp,location_name,location_count,mix_modify,local_location_id
FROM nutrition.mix 
LEFT JOIN nutrition.recipe ON recipe.id=mix.recipe_id
LEFT JOIN bovinemanagement.location ON mix.location_id=location.id
WHERE mix.id=func_mix_id;

SELECT recipe_cost_per_cow*mix_modify,recipe_kg_day_dry_per_cow*mix_modify,recipe_as_fed_kg_per_cow*mix_modify,recipe_moisture_percent,recipe_as_fed_kg_per_cow*mix_modify*location_count FROM nutrition.recipe_per_cow_info(recipe_id,func_timestamp_of_interest)  INTO  mix_cost_per_cow, mix_kg_day_dry_per_cow, mix_as_fed_kg_per_cow, mix_moisture_percent, total_mix_as_fed_kg;



/* we need to look up the next recipe after this one for a given location. */
/* this tells us the recipe stop date */
SELECT recipe.date FROM nutrition.recipe LEFT JOIN nutrition.mix ON recipe.id=mix.recipe_id WHERE mix.location_id=local_location_id and recipe.date > var_recipe_date ORDER BY recipe.date ASC LIMIT 1 INTO var_next_recipe_date;
recipe_stop_timestamp:=var_next_recipe_date;

/*check if the date is for after the recipe was retired. */
/*check if it is a future date, only useful with the latest recipes */
IF recipe_stop_timestamp < func_timestamp_of_interest
THEN
/*RAISE EXCEPTION 'function mix_info (2): Input timestamp: % is greater than the recipe stop timestamp: %.',func_timestamp_of_interest,recipe_stop_timestamp;*/
END IF;

SELECT mix_id,
       recipe_id, 
       recipe_start_timestamp,
       recipe_stop_timestamp,
       timestamp_of_interest,
       local_location_id,
       location_name,
       location_count,
       mix_cost_per_cow,
       mix_kg_day_dry_per_cow,
       mix_as_fed_kg_per_cow, 
       mix_moisture_percent,
       mix_modify,
       total_mix_as_fed_kg
INTO answer;

return next answer;

END;$$;


--
-- Name: mix_recipe_item_info(integer, integer); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.mix_recipe_item_info(func_recipe_id integer, func_recipe_item_id integer) RETURNS nutrition.mix_recipe_item_info_result
    LANGUAGE plpgsql
    AS $$DECLARE 
answer nutrition.mix_recipe_item_info_result;
var_recipe_weighted_count numeric;
var_kg_day_dry numeric;
var_dry_matter_percent numeric;
var_recipe_ingredient_wet_amount numeric;
var_recipe_ingredient_dry_amount numeric;

BEGIN

/* now find how many cows in each group */
/* then find the weighted cows in each group by summing location count and mix modify */ 
With tt as (
SELECT id as mix_id, bovinemanagement.location_total_at_timestamp (location_id, 'now'),modification_factor
FROM nutrition.mix
LEFT JOIN nutrition.mix_modify ON mix.id=mix_modify.mix_id
WHERE recipe_id= func_recipe_id
)
SELECT INTO var_recipe_weighted_count sum(location_total_at_timestamp*modification_factor) FROM tt;



/* now find kg_dry for recipe_item */

SELECT INTO var_kg_day_dry,var_dry_matter_percent kg_day_dry,(nutrition.ingredient_info(recipe_item.feedcurr_id,'now')).dry_matter_percent
FROM nutrition.recipe_item WHERE id=func_recipe_item_id;

var_recipe_ingredient_wet_amount:= var_kg_day_dry*1/((var_dry_matter_percent)*.01)*var_recipe_weighted_count;
var_recipe_ingredient_dry_amount:= var_kg_day_dry*var_recipe_weighted_count;

SELECT func_recipe_item_id,
      var_recipe_weighted_count,
var_kg_day_dry,
var_dry_matter_percent,
var_recipe_ingredient_wet_amount,
var_recipe_ingredient_dry_amount
INTO answer;

return answer;


END;$$;


--
-- Name: FUNCTION mix_recipe_item_info(func_recipe_id integer, func_recipe_item_id integer); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.mix_recipe_item_info(func_recipe_id integer, func_recipe_item_id integer) IS 'Takes recipe_id and a specific item in the recipe and returns the number of weighted cows and how much to feed of that item.';


--
-- Name: nrc_recipe_display(integer); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.nrc_recipe_display(func_nrc_recipe_id integer) RETURNS TABLE(feedcurr_id text, nrc_recipe integer, feed_library_name text, location character varying, feed_type character varying, kg_day_dry numeric, moisture_percent numeric, kg_day_wet numeric, kg_day_cost numeric, reverse integer, location_count_arr integer[], location_mod_count_arr numeric[], location_id_arr integer[], kg_day_dry_sum numeric, kg_day_wet_sum numeric, kg_day_cost_sum numeric, modified_mix_total_count numeric, kg_day_wet_mix numeric)
    LANGUAGE plpgsql
    AS $$
    
    BEGIN
     
         RETURN QUERY
		
with temp as (
SELECT nrc_recipe_item.feedcurr_id,nrc_recipe_item.nrc_recipe,nrc_recipe_item.feed_library_name, feedcurr.location,feedcurr.feed_type,nrc_recipe_item.kg_day_dry,round(100-dry_matter_percent,1) as moisture_percent,
nrc_recipe_item.kg_day_dry*1/(.01* dry_matter_percent) as kg_day_wet,nrc_recipe_item.kg_day_dry*.001*cost as kg_day_cost,strpos(nrc_recipe_item.feedcurr_id, 'bag_r') as reverse,

(array(SELECT(bovinemanagement.location_total_at_timestamp (location_id,now()::timestamp)) as location_mod_count  FROM nutrition.nrc_recipe_location WHERE nrc_recipe_location.nrc_recipe=47537   ORDER BY location_id ))  as location_count_arr,

(array(SELECT(bovinemanagement.location_total_at_timestamp (location_id,now()::timestamp)*modifier) as location_mod_count  FROM nutrition.nrc_recipe_location WHERE nrc_recipe_location.nrc_recipe=func_nrc_recipe_id   ORDER BY location_id ))  as location_mod_count_arr,

array(SELECT location_id FROM nutrition.nrc_recipe_location WHERE nrc_recipe_location.nrc_recipe=func_nrc_recipe_id  ORDER BY location_id) as location_id_arr

FROM nutrition.nrc_recipe_item 
LEFT JOIN nutrition.feedcurr ON nrc_recipe_item.feedcurr_id=feedcurr.id
WHERE nrc_recipe_item.nrc_recipe=func_nrc_recipe_id
), temp2 as
( SELECT *, (SELECT sum(temp.kg_day_dry) from temp group by temp.nrc_recipe) as kg_day_dry_sum,(SELECT sum(temp.kg_day_wet) from temp group by temp.nrc_recipe) as kg_day_wet_sum,(SELECT sum(temp.kg_day_cost) from temp group by temp.nrc_recipe) as kg_day_cost_sum,(SELECT SUM(s) FROM UNNEST(temp.location_mod_count_arr) s) as modified_mix_total_count
FROM temp)
SELECT *,temp2.modified_mix_total_count*temp2.kg_day_wet as kg_day_wet_mix FROM temp2 ORDER BY temp2.kg_day_dry DESC
 ;
    END;
$$;


--
-- Name: FUNCTION nrc_recipe_display(func_nrc_recipe_id integer); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.nrc_recipe_display(func_nrc_recipe_id integer) IS 'Displays all the info needed to print out a recipe. only good for realtime as it uses current time';


--
-- Name: recipe_per_cow_info(integer, timestamp without time zone); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.recipe_per_cow_info(func_recipe_id integer, func_timestamp_of_interest timestamp without time zone) RETURNS SETOF nutrition.recipe_per_cow_info_result
    LANGUAGE plpgsql
    AS $$DECLARE 
answer nutrition.recipe_per_cow_info_result;

recipe_cost_per_cow numeric;
recipe_kg_day_dry_per_cow numeric;
recipe_as_fed_kg_per_cow numeric;
recipe_moisture_percent numeric;
var_recipe_date timestamp without time zone;

  

BEGIN

/* check if the func_timestamp_of_interest is in a valid date range. */
SELECT INTO var_recipe_date date FROM nutrition.recipe WHERE id=func_recipe_id;
IF var_recipe_date > func_timestamp_of_interest
THEN
RAISE EXCEPTION 'function recipe_per_cow_info: Input timestamp is before the recipe existed.';
END IF;


WITH tt AS (
SELECT recipe_item.recipe_id,kg_day_dry,
(SELECT dry_matter_percent FROM nutrition.ingredient_info(recipe_item.feedcurr_id,func_timestamp_of_interest)) as dry_matter_percent,
(SELECT cost FROM nutrition.ingredient_info(recipe_item.feedcurr_id,func_timestamp_of_interest)) as cost
 FROM nutrition.recipe_item WHERE recipe_id=func_recipe_id
), t2 as (
SELECT  tt.kg_day_dry,tt.recipe_id as recipe_id,tt.cost*.001*kg_day_dry as item_cost,tt.dry_matter_percent as dry_matter_percent,1/(tt.dry_matter_percent*.01)*tt.kg_day_dry as as_fed_kg  FROM tt
)

SELECT t2.recipe_id , sum(item_cost) ,sum(t2.kg_day_dry) ,sum(t2.as_fed_kg) ,(sum(t2.kg_day_dry)/sum(t2.as_fed_kg))*100 FROM t2 GROUP BY recipe_id  INTO answer;

return next answer;

END;$$;


--
-- Name: stamp_update_log(); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.stamp_update_log() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN

BEGIN
      INSERT INTO system.update_log VALUES (TG_TABLE_SCHEMA,TG_TABLE_NAME);

EXCEPTION WHEN unique_violation THEN

      UPDATE system.update_log SET last_update='now()' WHERE schema_name=TG_TABLE_SCHEMA AND table_name=TG_TABLE_NAME;

END;

RETURN NEW;

END;

$$;


--
-- Name: FUNCTION stamp_update_log(); Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON FUNCTION nutrition.stamp_update_log() IS 'a table calls this function via trigger whenever it is modified and it updates the timestamp log';


--
-- Name: update_time_column(); Type: FUNCTION; Schema: nutrition; Owner: -
--

CREATE FUNCTION nutrition.update_time_column() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN
	   NEW.update_time = now(); 
	   RETURN NEW;
	END;$$;


--
-- Name: feeder_number_to_pen_name(text); Type: FUNCTION; Schema: urban_feeder_foreign_tiere; Owner: -
--

CREATE FUNCTION urban_feeder_foreign_tiere.feeder_number_to_pen_name(func_in text) RETURNS text
    LANGUAGE sql
    AS $$SELECT pen_name FROM (SELECT * FROM (VALUES ('1.1.1.2', 'Pen A'), ('1.1.1.1', 'Pen B'), ('1.2.1.2', 'Pen C'),('1.2.1.1', 'Pen D')) AS t (feeder_number,pen_name)) as tt WHERE feeder_number=func_in limit 1
$$;


--
-- Name: FUNCTION feeder_number_to_pen_name(func_in text); Type: COMMENT; Schema: urban_feeder_foreign_tiere; Owner: -
--

COMMENT ON FUNCTION urban_feeder_foreign_tiere.feeder_number_to_pen_name(func_in text) IS 'changes from urban to pen name, eg 1.1.1.1 to Pen B';


--
-- Name: foreign_server_urban_feeder; Type: SERVER; Schema: -; Owner: -
--

CREATE SERVER foreign_server_urban_feeder FOREIGN DATA WRAPPER postgres_fdw OPTIONS (
    dbname 'u46_1_kunde',
    host '192.168.9.6',
    port '5432'
);


--
-- Name: USER MAPPING david SERVER foreign_server_urban_feeder; Type: USER MAPPING; Schema: -; Owner: -
--

CREATE USER MAPPING FOR david SERVER foreign_server_urban_feeder OPTIONS (
    password 'safaritheme911',
    "user" 'waddy'
);


--
-- Name: USER MAPPING r2d2 SERVER foreign_server_urban_feeder; Type: USER MAPPING; Schema: -; Owner: -
--

CREATE USER MAPPING FOR r2d2 SERVER foreign_server_urban_feeder OPTIONS (
    password 'safaritheme911',
    "user" 'waddy'
);


--
-- Name: cow; Type: TABLE; Schema: alpro; Owner: -
--

CREATE TABLE alpro.cow (
    bovine_id integer NOT NULL,
    date date NOT NULL,
    milking_number integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    milkyield numeric NOT NULL,
    duration interval NOT NULL,
    milktime timestamp without time zone NOT NULL,
    peakflow numeric NOT NULL,
    averflow numeric NOT NULL,
    mpcnumber integer NOT NULL,
    manualid boolean NOT NULL,
    manualdetatch boolean NOT NULL,
    overridekey boolean NOT NULL,
    reattatch boolean NOT NULL,
    idtimemm timestamp without time zone NOT NULL,
    idtimess integer NOT NULL,
    milktimess numeric NOT NULL,
    milkflow0_15 numeric NOT NULL,
    milkflow15_30 numeric NOT NULL,
    milkflow30_60 numeric NOT NULL,
    milkflow60_120 numeric NOT NULL,
    lowmilkfloperc numeric NOT NULL,
    takeoffflow numeric NOT NULL,
    batchno integer NOT NULL,
    manualkey boolean NOT NULL,
    nomilk integer
);


--
-- Name: TABLE cow; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON TABLE alpro.cow IS 'keeps in sync relevant info with alpro system';


--
-- Name: COLUMN cow.milking_number; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON COLUMN alpro.cow.milking_number IS 'milking number 1 or 2';


--
-- Name: cron; Type: TABLE; Schema: alpro; Owner: -
--

CREATE TABLE alpro.cron (
    uuid uuid NOT NULL,
    pid numeric NOT NULL,
    start_time timestamp with time zone NOT NULL,
    end_date timestamp with time zone
);


--
-- Name: TABLE cron; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON TABLE alpro.cron IS 'Tracks whether the alpro cron job that runs every minute is running or not.';


--
-- Name: COLUMN cron.start_time; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON COLUMN alpro.cron.start_time IS 'when script started';


--
-- Name: COLUMN cron.end_date; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON COLUMN alpro.cron.end_date IS 'this is null when program is running.';


--
-- Name: sort_gate_log; Type: TABLE; Schema: alpro; Owner: -
--

CREATE TABLE alpro.sort_gate_log (
    bovine_id integer NOT NULL,
    home_location_id integer NOT NULL,
    sorted_location_id integer,
    event_time timestamp without time zone,
    data_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE sort_gate_log; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON TABLE alpro.sort_gate_log IS 'stores sort gate log';


--
-- Name: sort_gate_log_curr; Type: VIEW; Schema: alpro; Owner: -
--

CREATE VIEW alpro.sort_gate_log_curr AS
 SELECT DISTINCT ON (sort_gate_log.bovine_id) sort_gate_log.bovine_id,
    sort_gate_log.home_location_id,
    sort_gate_log.sorted_location_id,
    sort_gate_log.event_time,
    sort_gate_log.data_time,
    sort_gate_log.create_time
   FROM alpro.sort_gate_log
  ORDER BY sort_gate_log.bovine_id, sort_gate_log.event_time DESC;


--
-- Name: VIEW sort_gate_log_curr; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON VIEW alpro.sort_gate_log_curr IS 'latest sort gate state for each cow.';


--
-- Name: sync; Type: TABLE; Schema: alpro; Owner: -
--

CREATE TABLE alpro.sync (
    id integer NOT NULL,
    db_schema character varying NOT NULL,
    db_table character varying NOT NULL,
    event_time timestamp without time zone DEFAULT now() NOT NULL,
    state boolean DEFAULT false NOT NULL,
    debug_in character varying,
    debug_out character varying,
    debug_log character varying,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE sync; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON TABLE alpro.sync IS 'tracks what tables in db are synced.';


--
-- Name: COLUMN sync.db_schema; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON COLUMN alpro.sync.db_schema IS 'usually bovinemanagement';


--
-- Name: COLUMN sync.event_time; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON COLUMN alpro.sync.event_time IS 'defaults to current db time';


--
-- Name: COLUMN sync.state; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON COLUMN alpro.sync.state IS 'TRUE if sync sucessful';


--
-- Name: COLUMN sync.debug_in; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON COLUMN alpro.sync.debug_in IS 'contents of in file';


--
-- Name: COLUMN sync.debug_out; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON COLUMN alpro.sync.debug_out IS 'contents of out file';


--
-- Name: COLUMN sync.debug_log; Type: COMMENT; Schema: alpro; Owner: -
--

COMMENT ON COLUMN alpro.sync.debug_log IS 'contents of log file';


--
-- Name: sync_debug_out_seq; Type: SEQUENCE; Schema: alpro; Owner: -
--

CREATE SEQUENCE alpro.sync_debug_out_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: sync_debug_out_seq; Type: SEQUENCE OWNED BY; Schema: alpro; Owner: -
--

ALTER SEQUENCE alpro.sync_debug_out_seq OWNED BY alpro.sync.debug_out;


--
-- Name: sync_id_seq; Type: SEQUENCE; Schema: alpro; Owner: -
--

CREATE SEQUENCE alpro.sync_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: sync_id_seq; Type: SEQUENCE OWNED BY; Schema: alpro; Owner: -
--

ALTER SEQUENCE alpro.sync_id_seq OWNED BY alpro.sync.id;


--
-- Name: ble_bovine_tag; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_bovine_tag (
    bovine_id integer,
    tag_id macaddr NOT NULL,
    event_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE ble_bovine_tag; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.ble_bovine_tag IS 'tracks which bovine has which ble tag through time.';


--
-- Name: ble_bovine_tag_curr; Type: VIEW; Schema: bas; Owner: -
--

CREATE VIEW bas.ble_bovine_tag_curr AS
 WITH temp AS (
         SELECT DISTINCT ble_bovine_tag.tag_id,
            first_value(ble_bovine_tag.bovine_id) OVER w AS bovine_id,
            first_value(ble_bovine_tag.event_time) OVER w AS event_time
           FROM bas.ble_bovine_tag
          WINDOW w AS (PARTITION BY ble_bovine_tag.tag_id ORDER BY ble_bovine_tag.event_time DESC ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)
        )
 SELECT temp.tag_id,
    temp.bovine_id,
    temp.event_time
   FROM temp
  WHERE (temp.bovine_id IS NOT NULL);


--
-- Name: VIEW ble_bovine_tag_curr; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON VIEW bas.ble_bovine_tag_curr IS 'currently active tags linked to bovine ';


--
-- Name: ble_tag_event; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_tag_event (
    event_time timestamp with time zone NOT NULL,
    base_id macaddr NOT NULL,
    tag_id macaddr NOT NULL,
    rssi smallint NOT NULL
);


--
-- Name: TABLE ble_tag_event; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.ble_tag_event IS 'records BLE tag events. ie for cow location and heat detection.';


--
-- Name: COLUMN ble_tag_event.event_time; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.ble_tag_event.event_time IS 'miscro second timestamp of when event occured.';


--
-- Name: COLUMN ble_tag_event.base_id; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.ble_tag_event.base_id IS 'mac adress  of the reader';


--
-- Name: COLUMN ble_tag_event.tag_id; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.ble_tag_event.tag_id IS 'tag unique id';


--
-- Name: COLUMN ble_tag_event.rssi; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.ble_tag_event.rssi IS 'rssi value in dBm';


--
-- Name: bovine; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.bovine (
    id integer NOT NULL,
    update_time timestamp without time zone DEFAULT now() NOT NULL,
    full_reg_number character varying(18),
    full_name character varying(30),
    colour character varying(3) DEFAULT 'b&w'::character varying,
    birth_date date NOT NULL,
    death_date date,
    sire_full_reg_number character varying(18) NOT NULL,
    dam_full_reg_number character varying(18) NOT NULL,
    local_number numeric(10,0),
    rfid_number numeric(16,0) NOT NULL,
    recipient_full_reg_number character varying(18),
    create_time timestamp without time zone NOT NULL,
    ownerid character varying DEFAULT 'W&C'::character varying NOT NULL,
    CONSTRAINT "check_valid_chars check" CHECK (((full_reg_number)::text ~ '^[a-zA-Z0-9_\-]+$'::text)),
    CONSTRAINT check_valid_chars_dam CHECK (((dam_full_reg_number)::text ~ '^[a-zA-Z0-9_\-]+$'::text)),
    CONSTRAINT check_valid_chars_rfid CHECK (((rfid_number)::text ~ '^[a-zA-Z0-9_\-]+$'::text)),
    CONSTRAINT check_valid_chars_sire CHECK (((sire_full_reg_number)::text ~ '^[a-zA-Z0-9_\-]+$'::text)),
    CONSTRAINT local_number_positive CHECK ((local_number > (0)::numeric)),
    CONSTRAINT valid_rfid_range_check CHECK ((bovinemanagement.rfid_valid(rfid_number) = true))
);


--
-- Name: TABLE bovine; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.bovine IS 'Main table for bovines (cows) that describes attributes that they all have in common.';


--
-- Name: COLUMN bovine.full_reg_number; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.bovine.full_reg_number IS 'Can be null for bull calves.';


--
-- Name: COLUMN bovine.full_name; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.bovine.full_name IS 'Can be null for bull calves.';


--
-- Name: COLUMN bovine.death_date; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.bovine.death_date IS 'Null if bovine is still alive. really herd exit date';


--
-- Name: COLUMN bovine.sire_full_reg_number; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.bovine.sire_full_reg_number IS 'Must always have a father.';


--
-- Name: COLUMN bovine.dam_full_reg_number; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.bovine.dam_full_reg_number IS 'Must always have a mother.';


--
-- Name: COLUMN bovine.local_number; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.bovine.local_number IS 'Can be null for bulls.';


--
-- Name: COLUMN bovine.rfid_number; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.bovine.rfid_number IS 'Everything must have an eartag and thus an RFID.';


--
-- Name: COLUMN bovine.recipient_full_reg_number; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.bovine.recipient_full_reg_number IS 'For recipient bovine that carriers ET.';


--
-- Name: COLUMN bovine.ownerid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.bovine.ownerid IS 'who owns the animal';


--
-- Name: all_tags_latest_event; Type: VIEW; Schema: bas; Owner: -
--

CREATE VIEW bas.all_tags_latest_event AS
 WITH temp AS (
         SELECT DISTINCT ROW(ble_bovine_tag_curr.bovine_id, x.event_time) AS "row",
            ble_bovine_tag_curr.bovine_id,
            x.event_time,
            bovine.local_number,
            bovine.full_name,
            x.tag_id,
            ( SELECT ble_tag_event.rssi
                   FROM bas.ble_tag_event
                  WHERE ((ble_tag_event.tag_id = x.tag_id) AND (ble_tag_event.base_id = 'b8:27:eb:38:ff:8d'::macaddr) AND (ble_tag_event.event_time = x.event_time))) AS base_ff8d_rssi,
            ( SELECT ble_tag_event.rssi
                   FROM bas.ble_tag_event
                  WHERE ((ble_tag_event.tag_id = x.tag_id) AND (ble_tag_event.base_id = 'b8:27:eb:cf:fe:95'::macaddr) AND (ble_tag_event.event_time = x.event_time))) AS base_fe95_rssi,
            ( SELECT ble_tag_event.rssi
                   FROM bas.ble_tag_event
                  WHERE ((ble_tag_event.tag_id = x.tag_id) AND (ble_tag_event.base_id = 'b8:27:eb:06:eb:c0'::macaddr) AND (ble_tag_event.event_time = x.event_time))) AS base_ebc0_rssi,
            ( SELECT ble_tag_event.rssi
                   FROM bas.ble_tag_event
                  WHERE ((ble_tag_event.tag_id = x.tag_id) AND (ble_tag_event.base_id = 'b8:27:eb:5c:b7:0e'::macaddr) AND (ble_tag_event.event_time = x.event_time))) AS base_b70e_rssi,
            ( SELECT ble_tag_event.rssi
                   FROM bas.ble_tag_event
                  WHERE ((ble_tag_event.tag_id = x.tag_id) AND (ble_tag_event.base_id = 'b8:27:eb:80:17:02'::macaddr) AND (ble_tag_event.event_time = x.event_time))) AS base_1702_rssi
           FROM ((bas.ble_tag_event x
             LEFT JOIN bas.ble_bovine_tag_curr ON ((ble_bovine_tag_curr.tag_id = x.tag_id)))
             LEFT JOIN bovinemanagement.bovine ON ((bovine.id = ble_bovine_tag_curr.bovine_id)))
          WHERE ((x.event_time < now()) AND (x.event_time > (now() - '00:03:20'::interval)))
        )
 SELECT temp.bovine_id,
    temp.local_number,
    temp.full_name,
    temp.tag_id,
    to_timestamp(avg(date_part('epoch'::text, temp.event_time))) AS event_time,
    round(avg(temp.base_ff8d_rssi), 2) AS base_ff8d_rssi,
    round(avg(temp.base_fe95_rssi), 2) AS base_fe95_rssi,
    round(avg(temp.base_ebc0_rssi), 2) AS base_ebc0_rssi,
    round(avg(temp.base_b70e_rssi), 2) AS base_b70e_rssi,
    round(avg(temp.base_1702_rssi), 2) AS base_1702_rssi
   FROM temp
  GROUP BY temp.bovine_id, temp.local_number, temp.full_name, temp.tag_id
  ORDER BY temp.local_number;


--
-- Name: VIEW all_tags_latest_event; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON VIEW bas.all_tags_latest_event IS 'shows latest signal level for base stations for all active tags and links to bovine if possible';


--
-- Name: ble_base; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_base (
    base_id macaddr NOT NULL,
    x numeric NOT NULL,
    y numeric NOT NULL,
    z numeric NOT NULL,
    comment text
);


--
-- Name: TABLE ble_base; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.ble_base IS 'records location of base in local coordinate space';


--
-- Name: COLUMN ble_base.x; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.ble_base.x IS 'X coordinate in local coordinate space';


--
-- Name: ble_beacon; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_beacon (
    beacon_id macaddr NOT NULL,
    x numeric NOT NULL,
    y numeric NOT NULL,
    z numeric NOT NULL,
    comment text,
    event_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE ble_beacon; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.ble_beacon IS 'records location of beacons (static tags) in local coordinate space';


--
-- Name: ble_other_tag; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_other_tag (
    tag_id macaddr NOT NULL,
    name text NOT NULL,
    event_time timestamp with time zone NOT NULL
);


--
-- Name: TABLE ble_other_tag; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.ble_other_tag IS 'holds other tags, ie for tracking things like assets.';


--
-- Name: ble_show_all_latest; Type: VIEW; Schema: bas; Owner: -
--

CREATE VIEW bas.ble_show_all_latest AS
 SELECT DISTINCT x.tag_id,
    ble_bovine_tag.bovine_id,
    bovine.local_number,
    bovine.full_name,
    ( SELECT max(ble_tag_event.event_time) AS max
           FROM bas.ble_tag_event
          WHERE (ble_tag_event.tag_id = x.tag_id)) AS latest_event_time
   FROM ((bas.ble_tag_event x
     LEFT JOIN bas.ble_bovine_tag ON ((ble_bovine_tag.tag_id = x.tag_id)))
     LEFT JOIN bovinemanagement.bovine ON ((bovine.id = ble_bovine_tag.bovine_id)))
  GROUP BY x.tag_id, ble_bovine_tag.bovine_id, bovine.local_number, bovine.full_name
  ORDER BY ( SELECT max(ble_tag_event.event_time) AS max
           FROM bas.ble_tag_event
          WHERE (ble_tag_event.tag_id = x.tag_id)) DESC;


--
-- Name: ble_tag_event_filtered; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_tag_event_filtered (
    tag_id macaddr NOT NULL,
    event_time timestamp without time zone NOT NULL,
    base_ff8d_rssi real,
    base_fe95_rssi real,
    base_ebc0_rssi real,
    base_b70e_rssi real,
    base_1702_rssi real
);


--
-- Name: TABLE ble_tag_event_filtered; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.ble_tag_event_filtered IS 'ble_tag_event filtered into useable form';


--
-- Name: ble_tag_event_filtered_tag_id_seq; Type: SEQUENCE; Schema: bas; Owner: -
--

CREATE SEQUENCE bas.ble_tag_event_filtered_tag_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: ble_tag_event_filtered_tag_id_seq; Type: SEQUENCE OWNED BY; Schema: bas; Owner: -
--

ALTER SEQUENCE bas.ble_tag_event_filtered_tag_id_seq OWNED BY bas.ble_tag_event_filtered.tag_id;


--
-- Name: ble_tag_event_filtered_variance_10min; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_tag_event_filtered_variance_10min (
    tag_id macaddr NOT NULL,
    event_time timestamp without time zone NOT NULL,
    var_ff8d_rssi real,
    var_fe95_rssi real,
    var_ebc0_rssi real,
    var_b70e_rssi real,
    sum_var real,
    var_1702_rssi real
);


--
-- Name: ble_tag_event_filtered_variance_20min; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_tag_event_filtered_variance_20min (
    tag_id macaddr NOT NULL,
    event_time timestamp without time zone NOT NULL,
    var_ff8d_rssi real,
    var_fe95_rssi real,
    var_ebc0_rssi real,
    var_b70e_rssi real,
    sum_var real,
    var_1702_rssi real
);


--
-- Name: TABLE ble_tag_event_filtered_variance_20min; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.ble_tag_event_filtered_variance_20min IS '20 min variance for each base and tag';


--
-- Name: ble_tag_event_filtered_variance_7day; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_tag_event_filtered_variance_7day (
    tag_id macaddr NOT NULL,
    event_time timestamp without time zone NOT NULL,
    var_ff8d_rssi real,
    var_fe95_rssi real,
    var_ebc0_rssi real,
    var_b70e_rssi real,
    var_1702_rssi real
);


--
-- Name: TABLE ble_tag_event_filtered_variance_7day; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.ble_tag_event_filtered_variance_7day IS 'Records the variance for each tag over 7 days, to give us a baseline to normalize with.';


--
-- Name: ble_tag_event_filtered_variance_final; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.ble_tag_event_filtered_variance_final (
    tag_id macaddr NOT NULL,
    event_time timestamp without time zone NOT NULL,
    sum_var real
);


--
-- Name: ble_tag_event_high_variance_2days; Type: VIEW; Schema: bas; Owner: -
--

CREATE VIEW bas.ble_tag_event_high_variance_2days AS
 WITH temp3 AS (
         SELECT DISTINCT ON (ble_tag_event_filtered_variance_20min.tag_id, (timezone('UTC'::text, to_timestamp(avg(date_part('epoch'::text, ble_tag_event_filtered_variance_20min.event_time)) OVER ww)))) ble_tag_event_filtered_variance_20min.tag_id,
            timezone('UTC'::text, to_timestamp(avg(date_part('epoch'::text, ble_tag_event_filtered_variance_20min.event_time)) OVER ww)) AS event_time,
            sum(ble_tag_event_filtered_variance_20min.sum_var) OVER ww AS sum_var
           FROM bas.ble_tag_event_filtered_variance_20min
          WHERE (ble_tag_event_filtered_variance_20min.event_time < timezone('UTC'::text, (now() - '04:26:40'::interval)))
          WINDOW ww AS (PARTITION BY ble_tag_event_filtered_variance_20min.tag_id, (to_timestamp((floor((date_part('epoch'::text, ble_tag_event_filtered_variance_20min.event_time) / (16000)::double precision)) * (16000)::double precision))) ORDER BY ble_tag_event_filtered_variance_20min.event_time ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)
        ), temp4 AS (
         SELECT temp3.tag_id,
            temp3.event_time,
            temp3.sum_var,
            avg(temp3.sum_var) OVER (PARTITION BY temp3.tag_id) AS avg
           FROM temp3
        )
 SELECT temp4.tag_id,
    temp4.event_time,
    temp4.sum_var,
    ble_bovine_tag_curr.bovine_id,
    bovine.local_number,
    bovine.full_name
   FROM ((temp4
     LEFT JOIN bas.ble_bovine_tag_curr ON ((temp4.tag_id = ble_bovine_tag_curr.tag_id)))
     LEFT JOIN bovinemanagement.bovine ON ((ble_bovine_tag_curr.bovine_id = bovine.id)))
  WHERE ((temp4.sum_var > (1000)::double precision) AND (temp4.event_time >= (now() - '2 days'::interval)))
  ORDER BY temp4.event_time;


--
-- Name: VIEW ble_tag_event_high_variance_2days; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON VIEW bas.ble_tag_event_high_variance_2days IS 'high variance over 1000, last 2 days';


--
-- Name: ble_tag_event_variance_all; Type: VIEW; Schema: bas; Owner: -
--

CREATE VIEW bas.ble_tag_event_variance_all AS
 WITH temp3 AS (
         SELECT DISTINCT ON (ble_tag_event_filtered_variance_20min.tag_id, (timezone('UTC'::text, to_timestamp(avg(date_part('epoch'::text, ble_tag_event_filtered_variance_20min.event_time)) OVER ww)))) ble_tag_event_filtered_variance_20min.tag_id,
            timezone('UTC'::text, to_timestamp(avg(date_part('epoch'::text, ble_tag_event_filtered_variance_20min.event_time)) OVER ww)) AS event_time,
            sum(ble_tag_event_filtered_variance_20min.sum_var) OVER ww AS sum_var
           FROM bas.ble_tag_event_filtered_variance_20min
          WHERE (ble_tag_event_filtered_variance_20min.event_time < timezone('UTC'::text, (now() - '04:26:40'::interval)))
          WINDOW ww AS (PARTITION BY ble_tag_event_filtered_variance_20min.tag_id, (to_timestamp((floor((date_part('epoch'::text, ble_tag_event_filtered_variance_20min.event_time) / (16000)::double precision)) * (16000)::double precision))) ORDER BY ble_tag_event_filtered_variance_20min.event_time ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)
        ), temp4 AS (
         SELECT temp3.tag_id,
            temp3.event_time,
            temp3.sum_var,
            avg(temp3.sum_var) OVER (PARTITION BY temp3.tag_id) AS avg
           FROM temp3
        )
 SELECT temp4.tag_id,
    temp4.event_time,
    temp4.sum_var,
    ble_bovine_tag_curr.bovine_id,
    bovine.local_number,
    bovine.full_name
   FROM ((temp4
     LEFT JOIN bas.ble_bovine_tag_curr ON ((temp4.tag_id = ble_bovine_tag_curr.tag_id)))
     LEFT JOIN bovinemanagement.bovine ON ((ble_bovine_tag_curr.bovine_id = bovine.id)));


--
-- Name: VIEW ble_tag_event_variance_all; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON VIEW bas.ble_tag_event_variance_all IS 'variance sum over 16000 seconds';


--
-- Name: ble_valid_tag_anytime; Type: VIEW; Schema: bas; Owner: -
--

CREATE VIEW bas.ble_valid_tag_anytime AS
 SELECT ble_bovine_tag.tag_id
   FROM bas.ble_bovine_tag
UNION
 SELECT ble_beacon.beacon_id AS tag_id
   FROM bas.ble_beacon
UNION
 SELECT ble_other_tag.tag_id
   FROM bas.ble_other_tag;


--
-- Name: VIEW ble_valid_tag_anytime; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON VIEW bas.ble_valid_tag_anytime IS 'gives a list of tags that were valid at any point in time.';


--
-- Name: calf_barn_scale_event_raw; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.calf_barn_scale_event_raw (
    event_time timestamp without time zone NOT NULL,
    scale_id character(7) NOT NULL,
    mass numeric NOT NULL,
    temp numeric
);


--
-- Name: TABLE calf_barn_scale_event_raw; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.calf_barn_scale_event_raw IS 'records a few days worth, before processed.';


--
-- Name: calf_barn_scale_corrected; Type: VIEW; Schema: bas; Owner: -
--

CREATE VIEW bas.calf_barn_scale_corrected AS
 SELECT calf_barn_scale_event_raw.event_time,
    calf_barn_scale_event_raw.scale_id,
    calf_barn_scale_event_raw.mass,
    calf_barn_scale_event_raw.temp,
    (((('-1'::integer)::numeric * calf_barn_scale_event_raw.mass) - 34032.6) * 0.066737) AS mass_corrected
   FROM bas.calf_barn_scale_event_raw
  WHERE (calf_barn_scale_event_raw.scale_id = '1.1.1.1'::bpchar)
UNION
 SELECT calf_barn_scale_event_raw.event_time,
    calf_barn_scale_event_raw.scale_id,
    calf_barn_scale_event_raw.mass,
    calf_barn_scale_event_raw.temp,
    (((('-1'::integer)::numeric * calf_barn_scale_event_raw.mass) - 22637.7) * 0.37696) AS mass_corrected
   FROM bas.calf_barn_scale_event_raw
  WHERE (calf_barn_scale_event_raw.scale_id = '1.1.1.2'::bpchar)
  ORDER BY 1 DESC;


--
-- Name: VIEW calf_barn_scale_corrected; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON VIEW bas.calf_barn_scale_corrected IS 'calibrated scale weights';


--
-- Name: feed_auger_event_range; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.feed_auger_event_range (
    tank_id integer NOT NULL,
    event_range tsrange NOT NULL
);


--
-- Name: TABLE feed_auger_event_range; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.feed_auger_event_range IS 'stores feed tank auger time range on.';


SET default_with_oids = true;

--
-- Name: feed_bin_weight_event; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.feed_bin_weight_event (
    tank_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    cells numeric[] NOT NULL
);


--
-- Name: TABLE feed_bin_weight_event; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.feed_bin_weight_event IS 'shows load cell data for feed bins.';


--
-- Name: COLUMN feed_bin_weight_event.cells; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.feed_bin_weight_event.cells IS 'load cell voltage values';


SET default_with_oids = false;

--
-- Name: tmr_event; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.tmr_event (
    event_time timestamp without time zone DEFAULT ('now'::text)::timestamp without time zone NOT NULL,
    display_weight integer NOT NULL,
    gross_weight integer NOT NULL,
    lat double precision,
    lon double precision,
    hdop numeric
);


--
-- Name: TABLE tmr_event; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.tmr_event IS 'records events every second from tmr mixer. this table needs cleaned out quite often!';


--
-- Name: COLUMN tmr_event.event_time; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.tmr_event.event_time IS 'postgres time of event';


--
-- Name: COLUMN tmr_event.display_weight; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.tmr_event.display_weight IS 'in kg';


--
-- Name: COLUMN tmr_event.gross_weight; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.tmr_event.gross_weight IS 'in kg';


--
-- Name: COLUMN tmr_event.lat; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.tmr_event.lat IS 'latitude';


--
-- Name: COLUMN tmr_event.lon; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.tmr_event.lon IS 'longitude';


--
-- Name: COLUMN tmr_event.hdop; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.tmr_event.hdop IS 'gps hdop';


--
-- Name: vfd_event; Type: TABLE; Schema: bas; Owner: -
--

CREATE TABLE bas.vfd_event (
    vfd_id character(4) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    address_number numeric,
    address_value numeric
);


--
-- Name: TABLE vfd_event; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON TABLE bas.vfd_event IS 'variable frequency drive information from web over time.';


--
-- Name: COLUMN vfd_event.vfd_id; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.vfd_event.vfd_id IS 'short description, such as hamm';


--
-- Name: COLUMN vfd_event.event_time; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.vfd_event.event_time IS 'time of event';


--
-- Name: COLUMN vfd_event.address_number; Type: COMMENT; Schema: bas; Owner: -
--

COMMENT ON COLUMN bas.vfd_event.address_number IS 'different addresses on vfd.';


--
-- Name: aggregate_site_data; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.aggregate_site_data (
    full_reg_number character varying(18) NOT NULL,
    update_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    cdn_mobile_data jsonb,
    cdn_pedigree_data jsonb,
    cdn_genomics_data jsonb,
    hol_webservice_data jsonb,
    hol_conformation_data jsonb
);


--
-- Name: TABLE aggregate_site_data; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.aggregate_site_data IS 'holds parsed data from CDN mobile site and CDN main site and Holstein Canada site. DO NOT READ FROM THIS DIRECTLY. USE A VIEW.';


--
-- Name: COLUMN aggregate_site_data.cdn_mobile_data; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.aggregate_site_data.cdn_mobile_data IS 'CDN mobile site data as JSON';


--
-- Name: COLUMN aggregate_site_data.cdn_pedigree_data; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.aggregate_site_data.cdn_pedigree_data IS 'CDN pedigree page data as JSON';


--
-- Name: COLUMN aggregate_site_data.cdn_genomics_data; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.aggregate_site_data.cdn_genomics_data IS 'CDN genomics page data as JSON';


--
-- Name: COLUMN aggregate_site_data.hol_webservice_data; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.aggregate_site_data.hol_webservice_data IS 'HOL soap web-service data as JSON';


--
-- Name: COLUMN aggregate_site_data.hol_conformation_data; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.aggregate_site_data.hol_conformation_data IS 'HOL conformation page data as JSON';


--
-- Name: aggregate_view_all; Type: VIEW; Schema: batch; Owner: -
--

CREATE VIEW batch.aggregate_view_all AS
 SELECT aggregate_site_data.full_reg_number,
    aggregate_site_data.create_time,
    aggregate_site_data.update_time,
    ( SELECT true AS x
           FROM batch.aggregate_site_data xx
          WHERE ((aggregate_site_data.cdn_mobile_data IS NOT NULL) AND (aggregate_site_data.cdn_pedigree_data IS NOT NULL) AND (aggregate_site_data.cdn_genomics_data IS NOT NULL) AND (aggregate_site_data.hol_webservice_data IS NOT NULL) AND (aggregate_site_data.hol_conformation_data IS NOT NULL) AND ((xx.full_reg_number)::text = (aggregate_site_data.full_reg_number)::text))) AS authentic,
    NULLIF(COALESCE(NULLIF((aggregate_site_data.hol_webservice_data ->> 'full_name'::text), ''::text), NULLIF((aggregate_site_data.cdn_mobile_data ->> 'full_name'::text), ''::text)), ''::text) AS full_name,
    (NULLIF((aggregate_site_data.hol_webservice_data ->> 'birth_date'::text), ''::text))::date AS birth_date,
    "substring"((aggregate_site_data.full_reg_number)::text, 6, 1) AS sex,
    NULLIF((aggregate_site_data.hol_webservice_data ->> 'dam_full_reg_number'::text), ''::text) AS dam_full_reg_number,
    NULLIF((aggregate_site_data.hol_webservice_data ->> 'sire_full_reg_number'::text), ''::text) AS sire_full_reg_number,
    (aggregate_site_data.cdn_mobile_data ->> 'semen_code'::text) AS semen_code,
    (aggregate_site_data.cdn_mobile_data ->> 'male_short_name'::text) AS male_short_name,
    (NULLIF(((aggregate_site_data.cdn_mobile_data -> 'genetic_index'::text) ->> 'LPI'::text), ''::text))::numeric AS lpi,
    (NULLIF(((aggregate_site_data.cdn_mobile_data -> 'genetic_index'::text) ->> 'Pro$'::text), ''::text))::numeric AS prodoll,
    (NULLIF(((aggregate_site_data.cdn_mobile_data -> 'genetic_index'::text) ->> 'Reliability'::text), ''::text))::numeric AS reliability,
    (aggregate_site_data.cdn_genomics_data ->> 'genotype_panel'::text) AS geno_test,
    NULLIF((aggregate_site_data.hol_webservice_data ->> 'class'::text), ''::text) AS class,
    (NULLIF((aggregate_site_data.hol_webservice_data ->> 'score'::text), ''::text))::numeric AS score
   FROM batch.aggregate_site_data;


--
-- Name: VIEW aggregate_view_all; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON VIEW batch.aggregate_view_all IS 'everything in aggregate.';


--
-- Name: aggregate_view_curr; Type: VIEW; Schema: batch; Owner: -
--

CREATE VIEW batch.aggregate_view_curr AS
 SELECT bovine.id AS bovine_id,
    bovine.full_reg_number,
    aggregate_site_data.create_time,
    aggregate_site_data.update_time,
    ( SELECT true AS x
           FROM batch.aggregate_site_data xx
          WHERE ((aggregate_site_data.cdn_mobile_data IS NOT NULL) AND (aggregate_site_data.cdn_pedigree_data IS NOT NULL) AND (aggregate_site_data.cdn_genomics_data IS NOT NULL) AND (aggregate_site_data.hol_webservice_data IS NOT NULL) AND (aggregate_site_data.hol_conformation_data IS NOT NULL) AND ((xx.full_reg_number)::text = (aggregate_site_data.full_reg_number)::text))) AS authentic,
    (aggregate_site_data.hol_webservice_data ->> 'full_name'::text) AS full_name,
    ((aggregate_site_data.hol_webservice_data ->> 'birth_date'::text))::date AS birth_date,
    "substring"((aggregate_site_data.full_reg_number)::text, 6, 1) AS sex,
    (aggregate_site_data.hol_webservice_data ->> 'dam_full_reg_number'::text) AS dam_full_reg_number,
    (aggregate_site_data.hol_webservice_data ->> 'sire_full_reg_number'::text) AS sire_full_reg_number,
    (NULLIF(((aggregate_site_data.cdn_mobile_data -> 'genetic_index'::text) ->> 'LPI'::text), ''::text))::numeric AS lpi,
    (NULLIF(((aggregate_site_data.cdn_mobile_data -> 'genetic_index'::text) ->> 'Pro$'::text), ''::text))::numeric AS prodoll,
    (NULLIF(((aggregate_site_data.cdn_mobile_data -> 'genetic_index'::text) ->> 'Reliability'::text), ''::text))::numeric AS reliability,
    (aggregate_site_data.cdn_genomics_data ->> 'genotype_panel'::text) AS geno_test,
    (aggregate_site_data.hol_webservice_data ->> 'reg_status'::text) AS reg_status,
    ((aggregate_site_data.hol_webservice_data ->> 'reg_date'::text))::date AS reg_date,
    (aggregate_site_data.hol_webservice_data ->> 'class'::text) AS class,
    (aggregate_site_data.hol_webservice_data ->> 'class_all'::text) AS class_all,
    (NULLIF((aggregate_site_data.hol_webservice_data ->> 'score'::text), ''::text))::numeric AS score,
    (NULLIF(((aggregate_site_data.cdn_pedigree_data -> 'haplotypes'::text) ->> 'HCD'::text), ''::text))::numeric AS hcd1,
    (NULLIF(((aggregate_site_data.cdn_pedigree_data -> 'haplotypes'::text) ->> 'HH1'::text), ''::text))::numeric AS hh1,
    (NULLIF(((aggregate_site_data.cdn_pedigree_data -> 'haplotypes'::text) ->> 'HH2'::text), ''::text))::numeric AS hh2,
    (NULLIF(((aggregate_site_data.cdn_pedigree_data -> 'haplotypes'::text) ->> 'HH3'::text), ''::text))::numeric AS hh3,
    (NULLIF(((aggregate_site_data.cdn_pedigree_data -> 'haplotypes'::text) ->> 'HH4'::text), ''::text))::numeric AS hh4,
    (NULLIF(((aggregate_site_data.cdn_pedigree_data -> 'haplotypes'::text) ->> 'HH5'::text), ''::text))::numeric AS hh5
   FROM (bovinemanagement.bovine
     LEFT JOIN batch.aggregate_site_data ON (((bovine.full_reg_number)::text = (aggregate_site_data.full_reg_number)::text)));


--
-- Name: VIEW aggregate_view_curr; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON VIEW batch.aggregate_view_curr IS 'all alive or dead in system';


--
-- Name: alpro_group_production; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.alpro_group_production (
    event_time timestamp without time zone NOT NULL,
    location_id integer NOT NULL,
    avg_milk_production numeric NOT NULL,
    total_milk_production numeric
);


--
-- Name: TABLE alpro_group_production; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.alpro_group_production IS 'stores milk production by group over time.';


--
-- Name: beef_report; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.beef_report (
    id integer NOT NULL,
    event_date date NOT NULL,
    data jsonb
);


--
-- Name: TABLE beef_report; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.beef_report IS 'holds information on cull cow and bob calf auctions.';


--
-- Name: COLUMN beef_report.event_date; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.beef_report.event_date IS 'day of auction';


--
-- Name: COLUMN beef_report.data; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.beef_report.data IS 'auction data in json';


--
-- Name: beef_report_id_seq; Type: SEQUENCE; Schema: batch; Owner: -
--

CREATE SEQUENCE batch.beef_report_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: beef_report_id_seq; Type: SEQUENCE OWNED BY; Schema: batch; Owner: -
--

ALTER SEQUENCE batch.beef_report_id_seq OWNED BY batch.beef_report.id;


--
-- Name: beef_report_quebec_all_auctions; Type: VIEW; Schema: batch; Owner: -
--

CREATE VIEW batch.beef_report_quebec_all_auctions AS
 SELECT (((beef_report.data -> 'Cull cattle'::text) ->> 'date'::text))::date AS date,
    ((beef_report.data -> 'Cull cattle'::text) ->> 'volume'::text) AS cull_cow_volume,
    ((beef_report.data -> 'Cull cattle'::text) ->> 'avg_price'::text) AS cull_cow_avg_price,
    ((beef_report.data -> 'Holstein good male calves (90-120 lb)'::text) ->> 'volume'::text) AS good_male_calf_90_120_volume,
    ((beef_report.data -> 'Holstein good male calves (90-120 lb)'::text) ->> 'avg_price'::text) AS good_male_calf_90_120_avg_price,
    ((beef_report.data -> 'Holstein good male calves (120-140 lb)'::text) ->> 'volume'::text) AS good_male_calf_121_140_avg_volume,
    ((beef_report.data -> 'Holstein good male calves (120-140 lb)'::text) ->> 'avg_price'::text) AS good_male_calf_121_140_avg_price,
    ((beef_report.data -> 'Holstein average male calves (90-140 lb)'::text) ->> 'volume'::text) AS avg_male_calf_90_140_avg_volume,
    ((beef_report.data -> 'Holstein average male calves (90-140 lb)'::text) ->> 'avg_price'::text) AS avg_male_calf_90_140_avg_price,
    ((beef_report.data -> 'Holstein divers calves (all weights)'::text) ->> 'volume'::text) AS misc_calf_all_weights_avg_volume,
    ((beef_report.data -> 'Holstein divers calves (all weights)'::text) ->> 'avg_price'::text) AS misc_calf_all_weights_avg_price
   FROM batch.beef_report
  ORDER BY (((beef_report.data -> 'Cull cattle'::text) ->> 'date'::text))::date;


--
-- Name: bovine_milking_number_matview; Type: MATERIALIZED VIEW; Schema: batch; Owner: -
--

CREATE MATERIALIZED VIEW batch.bovine_milking_number_matview AS
 SELECT (a.val).event_date AS event_date,
    (a.val).sum_dim AS sum_dim,
    (a.val).total_milking AS total_milking,
    (a.val).total_milking_high AS total_milking_high,
    (a.val).total_milking_low AS total_milking_low,
    (a.val).total_milking_heifer AS total_milking_heifer
   FROM ( SELECT batch.bovine_milking_number_for_date((generate_series(('2010-02-01'::date)::timestamp with time zone, (('now'::text)::date)::timestamp with time zone, '1 day'::interval))::date) AS bovine_milking_number_for_date) a(val)
  WITH NO DATA;


--
-- Name: eartag; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.eartag (
    id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    bovine_id integer NOT NULL,
    side character(5) NOT NULL,
    userid character(32) NOT NULL,
    ordered_event_time timestamp without time zone,
    CONSTRAINT eartag_side_check CHECK ((side = ANY (ARRAY['left'::bpchar, 'right'::bpchar])))
);


--
-- Name: COLUMN eartag.side; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.eartag.side IS 'left or right';


--
-- Name: eartag_left; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.eartag_left AS
 SELECT DISTINCT ON (bovine.id) eartag.bovine_id,
    eartag.id AS eartag_left_id,
    eartag.event_time AS eartag_left_event_time,
    'none'::character(64) AS eartag_left_name
   FROM (bovinemanagement.bovine
     LEFT JOIN bovinemanagement.eartag ON ((eartag.bovine_id = bovine.id)))
  WHERE (eartag.side = 'left'::bpchar)
  ORDER BY bovine.id, eartag.event_time DESC;


--
-- Name: eartag_right; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.eartag_right AS
 SELECT DISTINCT ON (bovine.id) eartag.bovine_id,
    eartag.id AS eartag_right_id,
    eartag.event_time AS eartag_right_event_time,
    'none'::character(64) AS eartag_right_name
   FROM (bovinemanagement.bovine
     LEFT JOIN bovinemanagement.eartag ON ((eartag.bovine_id = bovine.id)))
  WHERE (eartag.side = 'right'::bpchar)
  ORDER BY bovine.id, eartag.event_time DESC;


--
-- Name: lactation; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.lactation (
    id integer NOT NULL,
    bovine_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    fresh_date timestamp without time zone NOT NULL,
    dry_date timestamp without time zone,
    userid character(32) NOT NULL
);


--
-- Name: TABLE lactation; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.lactation IS 'table to track lactation for each cow';


--
-- Name: COLUMN lactation.userid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.lactation.userid IS 'username';


--
-- Name: lactationcurr; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.lactationcurr AS
 SELECT one.bovine_id,
    one.id AS lactation_id,
    one.fresh_date,
    one.dry_date
   FROM bovinemanagement.lactation one
  WHERE ((one.fresh_date = ( SELECT max(lactation.fresh_date) AS max
           FROM bovinemanagement.lactation
          WHERE (lactation.bovine_id = one.bovine_id))) AND (one.dry_date IS NULL))
  ORDER BY one.bovine_id;


--
-- Name: location; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.location (
    id integer NOT NULL,
    name character varying(256),
    active boolean DEFAULT true NOT NULL,
    dbase_name character(10),
    milking_location boolean DEFAULT false NOT NULL,
    on_farm boolean DEFAULT true NOT NULL,
    ccia_reportable character varying(64) DEFAULT NULL::character varying
);


--
-- Name: TABLE location; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.location IS 'table with list of farm locations';


--
-- Name: COLUMN location.id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.location.id IS 'primary index';


--
-- Name: COLUMN location.name; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.location.name IS 'name of location';


--
-- Name: COLUMN location.active; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.location.active IS 'are we still using the location or not';


--
-- Name: COLUMN location.dbase_name; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.location.dbase_name IS 'erase this column when we no longer use dBase.';


--
-- Name: COLUMN location.milking_location; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.location.milking_location IS 'says whether this location is solely for milk cows.';


--
-- Name: COLUMN location.on_farm; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.location.on_farm IS 'locations that are on the farm.';


--
-- Name: COLUMN location.ccia_reportable; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.location.ccia_reportable IS 'CCIA reportable movement type';


--
-- Name: location_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.location_event (
    id integer NOT NULL,
    create_time timestamp without time zone,
    update_time timestamp without time zone,
    bovine_id integer,
    location_id integer,
    event_time timestamp without time zone,
    userid character(32) NOT NULL,
    transaction_id numeric(14,0) DEFAULT NULL::numeric
);


--
-- Name: COLUMN location_event.transaction_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.location_event.transaction_id IS 'groups a whole location move together. ';


--
-- Name: locationcurr; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.locationcurr AS
 SELECT DISTINCT ON (bovine.id) bovine.id AS bovine_id,
    location_event.id AS location_event_id,
    location_event.event_time,
    location.id AS location_id,
    location.name,
    location.dbase_name,
    location.milking_location AS milk_cows_only,
    location.on_farm AS location_on_farm
   FROM ((bovinemanagement.bovine
     LEFT JOIN bovinemanagement.location_event ON ((location_event.bovine_id = bovine.id)))
     JOIN bovinemanagement.location ON ((location.id = location_event.location_id)))
  WHERE (bovine.death_date IS NULL)
  ORDER BY bovine.id, location_event.event_time DESC;


--
-- Name: bovinecurr; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.bovinecurr AS
 SELECT bovine.id,
    bovine.local_number,
    bovine.full_reg_number,
    bovine.full_name,
    bovine.sire_full_reg_number,
    bovine.dam_full_reg_number,
    locationcurr.event_time AS location_last_event_time,
    locationcurr.location_id,
    locationcurr.name AS location_name,
    locationcurr.dbase_name AS location_name_dbase,
    bovine.birth_date,
    bovine.rfid_number,
    lactationcurr.fresh_date,
    lactationcurr.dry_date,
    eartag_left.eartag_left_name AS eartag_left,
    eartag_right.eartag_right_name AS eartag_right
   FROM ((((bovinemanagement.bovine
     JOIN bovinemanagement.locationcurr ON ((bovine.id = locationcurr.bovine_id)))
     LEFT JOIN bovinemanagement.lactationcurr ON ((bovine.id = lactationcurr.bovine_id)))
     LEFT JOIN bovinemanagement.eartag_left ON ((eartag_left.bovine_id = bovine.id)))
     LEFT JOIN bovinemanagement.eartag_right ON ((eartag_right.bovine_id = bovine.id)))
  WHERE ((bovine.death_date IS NULL) AND (locationcurr.location_on_farm = true) AND (bovinemanagement.rfid_purebred(bovine.rfid_number) = true))
  GROUP BY bovine.id, bovine.local_number, locationcurr.event_time, locationcurr.location_id, locationcurr.name, locationcurr.dbase_name, bovine.full_reg_number, bovine.full_name, bovine.sire_full_reg_number, bovine.dam_full_reg_number, bovine.birth_date, bovine.rfid_number, lactationcurr.fresh_date, lactationcurr.dry_date, eartag_left.eartag_left_name, eartag_right.eartag_right_name;


--
-- Name: VIEW bovinecurr; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.bovinecurr IS 'purebred alive';


--
-- Name: bovinecurr_long; Type: MATERIALIZED VIEW; Schema: batch; Owner: -
--

CREATE MATERIALIZED VIEW batch.bovinecurr_long AS
 SELECT bovinecurr.id,
    ( SELECT batch.current_lactation_total_revenue(bovinecurr.id) AS current_lactation_total_revenue) AS lactation_revenue,
    COALESCE(round(misc.avg_array(ARRAY[batch.calculate_milk_revenue_per_cow_per_date(bovinecurr.id, ((CURRENT_DATE - '1 day'::interval))::date), batch.calculate_milk_revenue_per_cow_per_date(bovinecurr.id, ((CURRENT_DATE - '2 days'::interval))::date), batch.calculate_milk_revenue_per_cow_per_date(bovinecurr.id, ((CURRENT_DATE - '3 days'::interval))::date), batch.calculate_milk_revenue_per_cow_per_date(bovinecurr.id, ((CURRENT_DATE - '4 days'::interval))::date), batch.calculate_milk_revenue_per_cow_per_date(bovinecurr.id, ((CURRENT_DATE - '5 days'::interval))::date), batch.calculate_milk_revenue_per_cow_per_date(bovinecurr.id, ((CURRENT_DATE - '6 days'::interval))::date), batch.calculate_milk_revenue_per_cow_per_date(bovinecurr.id, ((CURRENT_DATE - '7 days'::interval))::date)]), 2), (0)::numeric) AS avg_7_day_revenue,
    ( SELECT batch.prodoll_birthyear_quintile_rank(bovinecurr.id, (date_part('year'::text, bovinecurr.birth_date))::integer) AS prodoll_birthyear_quintile_rank) AS prodoll_birthyear_quintile_rank
   FROM bovinemanagement.bovinecurr
  WITH NO DATA;


--
-- Name: ccia_reported; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.ccia_reported (
    id integer NOT NULL,
    location_event_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    sent jsonb NOT NULL,
    received jsonb NOT NULL
);


--
-- Name: TABLE ccia_reported; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.ccia_reported IS 'records if location_id for a ccia reportable event has occured. It records json sent and received during reporting.';


--
-- Name: COLUMN ccia_reported.sent; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.ccia_reported.sent IS 'what we sent to CCIA';


--
-- Name: COLUMN ccia_reported.received; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.ccia_reported.received IS 'WHAT CCIA returned to us.';


--
-- Name: ccia_reported_id_seq; Type: SEQUENCE; Schema: batch; Owner: -
--

CREATE SEQUENCE batch.ccia_reported_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: ccia_reported_id_seq; Type: SEQUENCE OWNED BY; Schema: batch; Owner: -
--

ALTER SEQUENCE batch.ccia_reported_id_seq OWNED BY batch.ccia_reported.id;


--
-- Name: cdn_progeny_potential_genetics; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.cdn_progeny_potential_genetics (
    dam_full_reg_number character varying(18) NOT NULL,
    sire_full_reg_number character varying(18) NOT NULL,
    lpi_code character varying(8) NOT NULL,
    lpi integer NOT NULL,
    percent_inbreeding numeric NOT NULL,
    milk integer NOT NULL,
    fat integer NOT NULL,
    protein integer NOT NULL,
    percent_fat numeric NOT NULL,
    percent_protein numeric NOT NULL,
    scs numeric NOT NULL,
    conf integer NOT NULL,
    ms integer NOT NULL,
    f_and_l integer NOT NULL,
    ds integer NOT NULL,
    rp integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    prodoll numeric
);


--
-- Name: TABLE cdn_progeny_potential_genetics; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.cdn_progeny_potential_genetics IS 'holds information from CDN website on what happens when we breed a certain sire to a cow or heifer (Inbreeding).';


--
-- Name: COLUMN cdn_progeny_potential_genetics.scs; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.cdn_progeny_potential_genetics.scs IS 'somatic cell score';


--
-- Name: COLUMN cdn_progeny_potential_genetics.conf; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.cdn_progeny_potential_genetics.conf IS 'confirmation';


--
-- Name: COLUMN cdn_progeny_potential_genetics.ms; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.cdn_progeny_potential_genetics.ms IS 'mammary system';


--
-- Name: COLUMN cdn_progeny_potential_genetics.f_and_l; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.cdn_progeny_potential_genetics.f_and_l IS 'feet & legs';


--
-- Name: COLUMN cdn_progeny_potential_genetics.prodoll; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.cdn_progeny_potential_genetics.prodoll IS 'pro$ index';


--
-- Name: semen_straw; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.semen_straw (
    id integer NOT NULL,
    semen_code character(18) NOT NULL,
    freeze_date text NOT NULL,
    tank character(1) NOT NULL,
    bin integer,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    transaction_id numeric(14,0) NOT NULL,
    discarded boolean DEFAULT false NOT NULL,
    breeding_event_id integer,
    reserved boolean DEFAULT false NOT NULL,
    supplier_code character varying(8) NOT NULL,
    invoice_cost numeric,
    ownerid character varying DEFAULT 'W&C'::character varying NOT NULL,
    projection boolean DEFAULT false NOT NULL
);


--
-- Name: TABLE semen_straw; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.semen_straw IS 'table that tracks individual semen straws.';


--
-- Name: COLUMN semen_straw.id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.id IS 'index';


--
-- Name: COLUMN semen_straw.freeze_date; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.freeze_date IS 'date on straw';


--
-- Name: COLUMN semen_straw.tank; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.tank IS 'A,B,C,D,etc.';


--
-- Name: COLUMN semen_straw.bin; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.bin IS '1 to 6, maybe more in bigger tank.';


--
-- Name: COLUMN semen_straw.discarded; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.discarded IS 'when true, the straw was not used to breed a cow, but wasted or discarded for some reason.';


--
-- Name: COLUMN semen_straw.breeding_event_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.breeding_event_id IS 'field filled when straw used to breed a cow.';


--
-- Name: COLUMN semen_straw.reserved; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.reserved IS 'reserved for future use, when True, will not be used.';


--
-- Name: COLUMN semen_straw.invoice_cost; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.invoice_cost IS 'invoiced cost per straw, does not include later discunts.';


--
-- Name: COLUMN semen_straw.ownerid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.ownerid IS 'who owns the semen straw';


--
-- Name: COLUMN semen_straw.projection; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.semen_straw.projection IS 'used to make a fake straw';


--
-- Name: sire; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.sire (
    id integer NOT NULL,
    full_reg_number character(18) NOT NULL,
    full_name character varying(128) NOT NULL,
    short_name character varying(16) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    display boolean DEFAULT true NOT NULL
);


--
-- Name: TABLE sire; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.sire IS 'Holds a list of sires used for breeding.';


--
-- Name: COLUMN sire.display; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.sire.display IS 'indicate if a bull is active or not.';


--
-- Name: sire_semen_code; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.sire_semen_code (
    semen_code character(11) NOT NULL,
    sire_full_reg_number character(18) NOT NULL,
    sexed_semen boolean NOT NULL,
    CONSTRAINT semen_code_length CHECK ((length(semen_code) > 0))
);


--
-- Name: TABLE sire_semen_code; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.sire_semen_code IS 'stores sexed and normal semen code for a sire.';


--
-- Name: cdn_progeny_potential_genetics_lpi2000; Type: VIEW; Schema: batch; Owner: -
--

CREATE VIEW batch.cdn_progeny_potential_genetics_lpi2000 AS
 SELECT bovinecurr.id AS bovine_id,
    bovinecurr.local_number,
    bovinecurr.full_name,
    sire.full_name AS sire_full_name,
    sire.short_name AS sire_short_name,
    cdn_progeny_potential_genetics.sire_full_reg_number,
    cdn_progeny_potential_genetics.lpi_code,
    cdn_progeny_potential_genetics.lpi,
    cdn_progeny_potential_genetics.percent_inbreeding,
    cdn_progeny_potential_genetics.milk,
    cdn_progeny_potential_genetics.fat,
    cdn_progeny_potential_genetics.protein,
    cdn_progeny_potential_genetics.percent_fat,
    cdn_progeny_potential_genetics.percent_protein,
    cdn_progeny_potential_genetics.scs,
    cdn_progeny_potential_genetics.conf,
    cdn_progeny_potential_genetics.ms,
    cdn_progeny_potential_genetics.f_and_l,
    cdn_progeny_potential_genetics.ds,
    cdn_progeny_potential_genetics.rp,
    cdn_progeny_potential_genetics.update_time,
    aggregate_view_curr.reliability AS reliability_percent,
    ( SELECT count(semen_straw.id) AS count
           FROM (bovinemanagement.semen_straw
             LEFT JOIN bovinemanagement.sire_semen_code ON ((sire_semen_code.semen_code = semen_straw.semen_code)))
          WHERE ((semen_straw.breeding_event_id IS NULL) AND (semen_straw.discarded IS FALSE) AND (sire_semen_code.sire_full_reg_number = (cdn_progeny_potential_genetics.sire_full_reg_number)::bpchar))) AS straw_count
   FROM (((batch.cdn_progeny_potential_genetics
     LEFT JOIN bovinemanagement.bovinecurr ON (((bovinecurr.full_reg_number)::text = (cdn_progeny_potential_genetics.dam_full_reg_number)::text)))
     LEFT JOIN bovinemanagement.sire ON ((sire.full_reg_number = (cdn_progeny_potential_genetics.sire_full_reg_number)::bpchar)))
     LEFT JOIN batch.aggregate_view_curr ON (((aggregate_view_curr.full_reg_number)::text = (cdn_progeny_potential_genetics.sire_full_reg_number)::text)))
  WHERE (cdn_progeny_potential_genetics.lpi > 2000);


--
-- Name: VIEW cdn_progeny_potential_genetics_lpi2000; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON VIEW batch.cdn_progeny_potential_genetics_lpi2000 IS 'shows all cows inbreeding calculations for cows over 2000 lpi.';


--
-- Name: commodity_report; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.commodity_report (
    event_time timestamp without time zone NOT NULL,
    ddgs_low numeric,
    ddgs_high numeric,
    barley numeric,
    oats numeric,
    wheat_low numeric,
    wheat_high numeric,
    soybeans_sorel numeric,
    soybeanmeal_hamilton_low numeric,
    soybeanmeal_hamilton_high numeric,
    corn_quebec numeric,
    corn_west_ontario numeric
);


--
-- Name: TABLE commodity_report; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.commodity_report IS 'stores info from the ontario commodity report.';


--
-- Name: quota; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.quota (
    id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    event_type character(32) NOT NULL,
    butterfat_change numeric NOT NULL,
    userid character(32) NOT NULL,
    CONSTRAINT quota_event_type_check CHECK ((event_type = ANY (ARRAY['buy_sell'::bpchar, 'board_adjustment'::bpchar])))
);


--
-- Name: TABLE quota; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.quota IS '4';


--
-- Name: COLUMN quota.event_time; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.quota.event_time IS 'date when quota holdings changed.';


--
-- Name: COLUMN quota.event_type; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.quota.event_type IS 'reason for change, ie. bought/sold or adjustment';


--
-- Name: COLUMN quota.butterfat_change; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.quota.butterfat_change IS 'in kg';


--
-- Name: quota_id_seq; Type: SEQUENCE; Schema: batch; Owner: -
--

CREATE SEQUENCE batch.quota_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: quota_id_seq; Type: SEQUENCE OWNED BY; Schema: batch; Owner: -
--

ALTER SEQUENCE batch.quota_id_seq OWNED BY batch.quota.id;


--
-- Name: credit; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.credit (
    id integer DEFAULT nextval('batch.quota_id_seq'::regclass) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    event_type character(32) NOT NULL,
    credit_change numeric NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE credit; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.credit IS 'credit exchange changes.';


--
-- Name: credit_exchange; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.credit_exchange (
    date date NOT NULL,
    price numeric NOT NULL,
    cleared numeric,
    offered numeric,
    userid character(32) NOT NULL
);


--
-- Name: TABLE credit_exchange; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.credit_exchange IS 'historical credit exchange prices';


--
-- Name: daily_number_cows_milking; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.daily_number_cows_milking (
    id integer NOT NULL,
    date timestamp without time zone NOT NULL,
    number_of_cows integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    average_dim numeric NOT NULL,
    total_milking_high integer,
    total_milking_low integer,
    total_milking_heifer integer
);


--
-- Name: TABLE daily_number_cows_milking; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.daily_number_cows_milking IS 'stores how many cows were milking on a certain date.';


--
-- Name: COLUMN daily_number_cows_milking.average_dim; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.daily_number_cows_milking.average_dim IS 'average DIM for this paticular day.';


--
-- Name: daily_number_cows_milking_id_seq; Type: SEQUENCE; Schema: batch; Owner: -
--

CREATE SEQUENCE batch.daily_number_cows_milking_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: daily_number_cows_milking_id_seq; Type: SEQUENCE OWNED BY; Schema: batch; Owner: -
--

ALTER SEQUENCE batch.daily_number_cows_milking_id_seq OWNED BY batch.daily_number_cows_milking.id;


--
-- Name: day_ingredient_usage_result; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.day_ingredient_usage_result (
    feedcurr_id text NOT NULL,
    date timestamp without time zone NOT NULL,
    dry_matter_percent numeric,
    day_ingredient_weighted_count numeric,
    day_ingredient_wet_amount numeric,
    day_ingredient_dry_amount numeric,
    feed_type_id integer
);


--
-- Name: days_in_month; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.days_in_month (
    date_part double precision
);


--
-- Name: employee_shift; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.employee_shift (
    userid character(32) NOT NULL,
    date date NOT NULL,
    shift character varying NOT NULL,
    milking_number integer NOT NULL
);


--
-- Name: TABLE employee_shift; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.employee_shift IS 'data from google calendar on who worked when.';


--
-- Name: error_curr; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.error_curr (
    id integer NOT NULL,
    hash character varying NOT NULL,
    create_time timestamp with time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    level integer NOT NULL,
    detail text NOT NULL,
    calling_class text NOT NULL,
    CONSTRAINT level_123 CHECK (((level = 1) OR (level = 2) OR (level = 3)))
);


--
-- Name: TABLE error_curr; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.error_curr IS 'stores current list of errors from notify obj.';


--
-- Name: error_curr_id_seq; Type: SEQUENCE; Schema: batch; Owner: -
--

CREATE SEQUENCE batch.error_curr_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: error_curr_id_seq; Type: SEQUENCE OWNED BY; Schema: batch; Owner: -
--

ALTER SEQUENCE batch.error_curr_id_seq OWNED BY batch.error_curr.id;


--
-- Name: holstein_canada_data; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.holstein_canada_data (
    client_id character varying(20),
    phone_number character varying(16),
    breed character varying(2),
    country character varying(3),
    sex character varying(1),
    reg_no character varying(12) NOT NULL,
    name character varying(30),
    birth_date date,
    colour character varying(3),
    prov_eartag bigint,
    onfarm_eartag bigint,
    rfid bigint,
    chain_num bigint,
    common_name character varying(12),
    classification_country character varying(3),
    classification character varying(2),
    classification_score bigint,
    multiple_ex bigint,
    sire_breed character varying(2),
    sire_country character varying(3),
    sire_sex character varying(1),
    sire_reg_no character varying(12),
    sire_name character varying(30),
    dam_breed character varying(2),
    dam_country character varying(3),
    dam_sex character varying(1),
    dam_reg_no character varying(12),
    dam_name character varying(30),
    dam_classification_country character varying(3),
    dam_classification character varying(2),
    dam_classification_score bigint,
    dam_multiple_ex bigint,
    mgs_breed character varying(2),
    mgs_country character varying(3),
    mgs_sex character varying(1),
    mgs_reg_no character varying(12),
    mgs_name character varying(30),
    prod_eval_date character varying(6),
    prod_type_of_proof bigint,
    prod_ebv_milk_kg bigint,
    prod_ranking_milk bigint,
    prod_ebv_fat_kg bigint,
    prod_ranking_fat bigint,
    prod_percent_fat numeric,
    prod_protein_kg bigint,
    prod_ranking_protein bigint,
    prod_percent_protein numeric,
    prod_reliability bigint,
    prod_scs numeric,
    prod_scs_reliability bigint,
    prod_inbreeding_coeff numeric,
    prod_r_value bigint,
    conf_eval_date character varying(6),
    conf_type_of_proof bigint,
    conf_ebv_conf bigint,
    conf_ranking bigint,
    conf_type_reliability bigint,
    conf_type_of_proof_scorecard bigint,
    conf_ebv_rp bigint,
    conf_ebv_ms bigint,
    conf_ebv_fl bigint,
    conf_ebv_ds bigint,
    conf_type_of_proof_secondary_traits bigint,
    conf_ebv_ps bigint,
    conf_ebv_ra bigint,
    conf_ebv_ra_code character varying(1),
    conf_ebv_pw bigint,
    conf_ebv_ls bigint,
    conf_ebv_ud bigint,
    conf_ebv_ud_code character varying(1),
    conf_ut bigint,
    conf_med bigint,
    conf_fa bigint,
    conf_ftp bigint,
    conf_ftp_code character varying(1),
    conf_rah bigint,
    conf_raw bigint,
    conf_rtp bigint,
    conf_rtp_code character varying(1),
    conf_tl bigint,
    conf_tl_code character varying(1),
    conf_ft bigint,
    conf_hd bigint,
    conf_bq bigint,
    conf_rl bigint,
    conf_rlsv bigint,
    conf_rlsv_code character varying(1),
    conf_rlrv bigint,
    conf_st bigint,
    conf_fe bigint,
    conf_cw bigint,
    conf_bd bigint,
    conf_angularity bigint,
    lpi bigint,
    lpi_ranking bigint,
    mgd_breed character varying(2),
    mgd_country character varying(3),
    mgd_sex character varying(1),
    mgd_reg_no character varying(12),
    mgd_name character varying(30),
    pgs_breed character varying(2),
    pgs_country character varying(3),
    pgs_sex character varying(1),
    pgs_reg_no character varying(12),
    pgs_name character varying(30),
    pgd_breed character varying(2),
    pgd_country character varying(3),
    pgd_sex character varying(1),
    pgd_reg_no character varying(12),
    pgd_name character varying(30),
    mggs_breed character varying(2),
    mggs_country character varying(3),
    mggs_sex character varying(1),
    mggs_reg_no character varying(12),
    mggs_name character varying(30),
    extract_date date NOT NULL,
    gebv character varying(1),
    pro_doll bigint
);


--
-- Name: TABLE holstein_canada_data; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.holstein_canada_data IS 'From Herd Book data files. Obsolete??';


--
-- Name: COLUMN holstein_canada_data.pro_doll; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.holstein_canada_data.pro_doll IS 'pro$ index';


--
-- Name: holstein_canada_data_curr; Type: VIEW; Schema: batch; Owner: -
--

CREATE VIEW batch.holstein_canada_data_curr AS
 SELECT ((((hol.breed)::text || (hol.country)::text) || (hol.sex)::text) || (hol.reg_no)::text) AS full_reg_number,
    hol.client_id,
    hol.phone_number,
    hol.breed,
    hol.country,
    hol.sex,
    hol.reg_no,
    hol.name,
    hol.birth_date,
    hol.colour,
    hol.prov_eartag,
    hol.onfarm_eartag,
    hol.rfid,
    hol.chain_num,
    hol.common_name,
    hol.classification_country,
    hol.classification,
    hol.classification_score,
    hol.multiple_ex,
    hol.sire_breed,
    hol.sire_country,
    hol.sire_sex,
    hol.sire_reg_no,
    hol.sire_name,
    hol.dam_breed,
    hol.dam_country,
    hol.dam_sex,
    hol.dam_reg_no,
    hol.dam_name,
    hol.dam_classification_country,
    hol.dam_classification,
    hol.dam_classification_score,
    hol.dam_multiple_ex,
    hol.mgs_breed,
    hol.mgs_country,
    hol.mgs_sex,
    hol.mgs_reg_no,
    hol.mgs_name,
    hol.prod_eval_date,
    hol.prod_type_of_proof,
    hol.prod_ebv_milk_kg,
    hol.prod_ranking_milk,
    hol.prod_ebv_fat_kg,
    hol.prod_ranking_fat,
    hol.prod_percent_fat,
    hol.prod_protein_kg,
    hol.prod_ranking_protein,
    hol.prod_percent_protein,
    hol.prod_reliability,
    hol.prod_scs,
    hol.prod_scs_reliability,
    hol.prod_inbreeding_coeff,
    hol.prod_r_value,
    hol.conf_eval_date,
    hol.conf_type_of_proof,
    hol.conf_ebv_conf,
    hol.conf_ranking,
    hol.conf_type_reliability,
    hol.conf_type_of_proof_scorecard,
    hol.conf_ebv_rp,
    hol.conf_ebv_ms,
    hol.conf_ebv_fl,
    hol.conf_ebv_ds,
    hol.conf_type_of_proof_secondary_traits,
    hol.conf_ebv_ps,
    hol.conf_ebv_ra,
    hol.conf_ebv_ra_code,
    hol.conf_ebv_pw,
    hol.conf_ebv_ls,
    hol.conf_ebv_ud,
    hol.conf_ebv_ud_code,
    hol.conf_ut,
    hol.conf_med,
    hol.conf_fa,
    hol.conf_ftp,
    hol.conf_ftp_code,
    hol.conf_rah,
    hol.conf_raw,
    hol.conf_rtp,
    hol.conf_rtp_code,
    hol.conf_tl,
    hol.conf_tl_code,
    hol.conf_ft,
    hol.conf_hd,
    hol.conf_bq,
    hol.conf_rl,
    hol.conf_rlsv,
    hol.conf_rlsv_code,
    hol.conf_rlrv,
    hol.conf_st,
    hol.conf_fe,
    hol.conf_cw,
    hol.conf_bd,
    hol.conf_angularity,
    hol.lpi,
    hol.pro_doll,
    hol.lpi_ranking,
    hol.mgd_breed,
    hol.mgd_country,
    hol.mgd_sex,
    hol.mgd_reg_no,
    hol.mgd_name,
    hol.pgs_breed,
    hol.pgs_country,
    hol.pgs_sex,
    hol.pgs_reg_no,
    hol.pgs_name,
    hol.pgd_breed,
    hol.pgd_country,
    hol.pgd_sex,
    hol.pgd_reg_no,
    hol.pgd_name,
    hol.mggs_breed,
    hol.mggs_country,
    hol.mggs_sex,
    hol.mggs_reg_no,
    hol.mggs_name,
    hol.extract_date,
    hol.gebv
   FROM batch.holstein_canada_data hol
  WHERE (((hol.reg_no)::text = (hol.reg_no)::text) AND (hol.extract_date = ( SELECT max(holstein_canada_data.extract_date) AS max
           FROM batch.holstein_canada_data
          WHERE ((hol.reg_no)::text = (holstein_canada_data.reg_no)::text))));


--
-- Name: VIEW holstein_canada_data_curr; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON VIEW batch.holstein_canada_data_curr IS 'latest data for each reg FROM HERD-BOOK FILES.';


--
-- Name: holstein_canada_registered; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.holstein_canada_registered (
    bovine_id integer NOT NULL,
    local_reg_status boolean NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    short_uuid character(17),
    event_time_reg_sent timestamp without time zone,
    parentage_verified boolean DEFAULT true NOT NULL
);


--
-- Name: TABLE holstein_canada_registered; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.holstein_canada_registered IS 'all bovines who are confirmed to be registered in Holstein Canada system.';


--
-- Name: COLUMN holstein_canada_registered.local_reg_status; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.holstein_canada_registered.local_reg_status IS 'place holder for syncing with aggregate';


--
-- Name: COLUMN holstein_canada_registered.short_uuid; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.holstein_canada_registered.short_uuid IS 'part of file name sent when animal registered.';


--
-- Name: COLUMN holstein_canada_registered.event_time_reg_sent; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.holstein_canada_registered.event_time_reg_sent IS 'time when registration was sent to Holstein Canada';


--
-- Name: COLUMN holstein_canada_registered.parentage_verified; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.holstein_canada_registered.parentage_verified IS 'is the parentage of the animal verified with holstein canada';


--
-- Name: incentive_day; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.incentive_day (
    id integer DEFAULT nextval('batch.quota_id_seq'::regclass) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    date date NOT NULL,
    incentive numeric NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: COLUMN incentive_day.incentive; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.incentive_day.incentive IS 'units of quota bf days';


--
-- Name: milk2020_hoof; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.milk2020_hoof (
    chain_number integer NOT NULL,
    trim_time timestamp with time zone NOT NULL,
    data jsonb NOT NULL
);


--
-- Name: milk_pickup_event; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.milk_pickup_event (
    id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    milk_amount_sold numeric NOT NULL,
    milk_amount_disbursed numeric NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE milk_pickup_event; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.milk_pickup_event IS 'holds information on milk picked up by the milk board.';


--
-- Name: COLUMN milk_pickup_event.milk_amount_sold; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.milk_pickup_event.milk_amount_sold IS 'milk amount tanken away by truck';


--
-- Name: COLUMN milk_pickup_event.milk_amount_disbursed; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.milk_pickup_event.milk_amount_disbursed IS 'estimated amount fed to calves and dumped from cows in milking line';


--
-- Name: milk_pickup_event_id_seq; Type: SEQUENCE; Schema: batch; Owner: -
--

CREATE SEQUENCE batch.milk_pickup_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: milk_pickup_event_id_seq; Type: SEQUENCE OWNED BY; Schema: batch; Owner: -
--

ALTER SEQUENCE batch.milk_pickup_event_id_seq OWNED BY batch.milk_pickup_event.id;


--
-- Name: milk_statement; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.milk_statement (
    date date NOT NULL,
    butterfat numeric NOT NULL,
    protein numeric NOT NULL,
    lactose numeric NOT NULL,
    transport numeric NOT NULL,
    promotion numeric NOT NULL,
    admin numeric NOT NULL,
    lab numeric NOT NULL,
    research numeric NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: COLUMN milk_statement.transport; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.milk_statement.transport IS 'per liter';


--
-- Name: nb_bulk_tank_sample; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.nb_bulk_tank_sample (
    id integer NOT NULL,
    create_time timestamp without time zone DEFAULT now() NOT NULL,
    fat numeric NOT NULL,
    protein numeric NOT NULL,
    lactose numeric NOT NULL,
    scc integer NOT NULL,
    test_period numeric(1,0) NOT NULL,
    test_time_start timestamp without time zone NOT NULL,
    test_time_finish timestamp without time zone NOT NULL,
    average_test_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE nb_bulk_tank_sample; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.nb_bulk_tank_sample IS 'stores weekly fat/scc test data';


--
-- Name: COLUMN nb_bulk_tank_sample.fat; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.nb_bulk_tank_sample.fat IS 'percentage';


--
-- Name: COLUMN nb_bulk_tank_sample.protein; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.nb_bulk_tank_sample.protein IS 'percentage';


--
-- Name: COLUMN nb_bulk_tank_sample.lactose; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.nb_bulk_tank_sample.lactose IS 'percentage';


--
-- Name: COLUMN nb_bulk_tank_sample.scc; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.nb_bulk_tank_sample.scc IS 'scc per ml';


--
-- Name: COLUMN nb_bulk_tank_sample.test_period; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON COLUMN batch.nb_bulk_tank_sample.test_period IS 'period of test 1,2,3 or 4.';


--
-- Name: provincial_milk_test_data_id_seq; Type: SEQUENCE; Schema: batch; Owner: -
--

CREATE SEQUENCE batch.provincial_milk_test_data_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: provincial_milk_test_data_id_seq; Type: SEQUENCE OWNED BY; Schema: batch; Owner: -
--

ALTER SEQUENCE batch.provincial_milk_test_data_id_seq OWNED BY batch.nb_bulk_tank_sample.id;


--
-- Name: nb_bulk_tank_health; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.nb_bulk_tank_health (
    id integer DEFAULT nextval('batch.provincial_milk_test_data_id_seq'::regclass) NOT NULL,
    create_time timestamp without time zone DEFAULT now() NOT NULL,
    spc numeric NOT NULL,
    lpc numeric NOT NULL,
    event_time date NOT NULL,
    tank character(1) NOT NULL
);


--
-- Name: provincial_milk_test_data_fat_seq; Type: SEQUENCE; Schema: batch; Owner: -
--

CREATE SEQUENCE batch.provincial_milk_test_data_fat_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: provincial_milk_test_data_fat_seq; Type: SEQUENCE OWNED BY; Schema: batch; Owner: -
--

ALTER SEQUENCE batch.provincial_milk_test_data_fat_seq OWNED BY batch.nb_bulk_tank_sample.fat;


--
-- Name: provincial_milk_test_data_protein_seq; Type: SEQUENCE; Schema: batch; Owner: -
--

CREATE SEQUENCE batch.provincial_milk_test_data_protein_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: provincial_milk_test_data_protein_seq; Type: SEQUENCE OWNED BY; Schema: batch; Owner: -
--

ALTER SEQUENCE batch.provincial_milk_test_data_protein_seq OWNED BY batch.nb_bulk_tank_sample.protein;


--
-- Name: quota_summary; Type: VIEW; Schema: batch; Owner: -
--

CREATE VIEW batch.quota_summary AS
 WITH temp AS (
         SELECT (date_trunc('day'::text, dd.dd))::date AS date_trunc
           FROM generate_series('2007-01-01 00:00:00'::timestamp without time zone, ((('now'::text)::date)::timestamp without time zone + '1 year'::interval), '1 mon'::interval) dd(dd)
        ), temp2 AS (
         SELECT temp.date_trunc AS date,
            batch.quota_on_date(temp.date_trunc) AS quota,
            batch.quota_incentive_amount_on_date(temp.date_trunc) AS quota_incentive
           FROM temp
        )
 SELECT temp2.date,
    temp2.quota,
    temp2.quota_incentive,
    (temp2.quota + temp2.quota_incentive) AS total_quota
   FROM temp2;


--
-- Name: VIEW quota_summary; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON VIEW batch.quota_summary IS 'shows monthly quota through time.';


--
-- Name: shurgain_invoice_sheet1; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.shurgain_invoice_sheet1 (
    data jsonb NOT NULL
);


--
-- Name: TABLE shurgain_invoice_sheet1; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.shurgain_invoice_sheet1 IS 'shurgain report for waddy & colpitts sheet 1 in json';


--
-- Name: shurgain_invoice_sheet2; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.shurgain_invoice_sheet2 (
    data jsonb NOT NULL
);


--
-- Name: TABLE shurgain_invoice_sheet2; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.shurgain_invoice_sheet2 IS 'shurgain report for waddy & colpitts sheet 2 in json
';


--
-- Name: shur_gain_invoice_data; Type: VIEW; Schema: batch; Owner: -
--

CREATE VIEW batch.shur_gain_invoice_data AS
 WITH temp AS (
         SELECT DISTINCT concat((shurgain_invoice_sheet2.data ->> 'Invoice #'::text), '_', (shurgain_invoice_sheet2.data ->> 'Item #'::text), '_', (shurgain_invoice_sheet2.data ->> 'Total Invoice (excl taxes)'::text)) AS index,
            (shurgain_invoice_sheet2.data ->> 'Invoice #'::text) AS invoice_num,
            (shurgain_invoice_sheet2.data ->> 'Item #'::text) AS item_num,
            ('1899-12-30'::date + (((shurgain_invoice_sheet2.data ->> 'Invoice date'::text) || ' DAYS'::text))::interval) AS event_time,
            (shurgain_invoice_sheet2.data ->> 'Item Description'::text) AS item_desc,
            ((shurgain_invoice_sheet2.data ->> 'Weight'::text))::numeric AS mass,
            ARRAY( SELECT (shurgain_invoice_sheet1.data ->> 'Sales price UoM'::text)
                   FROM batch.shurgain_invoice_sheet1
                  WHERE (((shurgain_invoice_sheet1.data ->> 'Item Number'::text) = (shurgain_invoice_sheet2.data ->> 'Item #'::text)) AND ((shurgain_invoice_sheet1.data ->> 'Invoice #'::text) = (shurgain_invoice_sheet2.data ->> 'Invoice #'::text)))) AS uom,
            ARRAY( SELECT (shurgain_invoice_sheet1.data ->> 'Tax code'::text)
                   FROM batch.shurgain_invoice_sheet1
                  WHERE (((shurgain_invoice_sheet1.data ->> 'Item Number'::text) = (shurgain_invoice_sheet2.data ->> 'Item #'::text)) AND ((shurgain_invoice_sheet1.data ->> 'Invoice #'::text) = (shurgain_invoice_sheet2.data ->> 'Invoice #'::text)))) AS tax_code,
            ((shurgain_invoice_sheet2.data ->> 'Total Invoice (excl taxes)'::text))::numeric AS price_total
           FROM batch.shurgain_invoice_sheet2
        )
 SELECT temp.index,
    temp.invoice_num,
    temp.item_num,
    temp.event_time,
    temp.item_desc,
    temp.mass,
    temp.uom,
    temp.tax_code,
    temp.price_total,
        CASE
            WHEN (temp.uom[1] = 'MT'::text) THEN (temp.mass * (1000)::numeric)
            WHEN (temp.uom[1] = 'K25'::text) THEN (temp.mass * (25)::numeric)
            WHEN (temp.uom[1] = 'BAG'::text) THEN (temp.mass * (25)::numeric)
            WHEN (temp.uom[1] = 'PCE'::text) THEN (temp.mass * (25)::numeric)
            ELSE temp.mass
        END AS mass_corrected,
        CASE
            WHEN (temp.uom[1] = 'MT'::text) THEN 'KG'::text
            WHEN (temp.uom[1] = 'K25'::text) THEN 'KG'::text
            WHEN (temp.uom[1] = 'BAG'::text) THEN 'KG'::text
            WHEN (temp.uom[1] = 'PCE'::text) THEN 'KG'::text
            ELSE temp.uom[1]
        END AS unit_measure_corrected
   FROM temp;


--
-- Name: valacta_raw; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.valacta_raw (
    chain_number integer NOT NULL,
    test_date date NOT NULL,
    data json NOT NULL
);


--
-- Name: TABLE valacta_raw; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.valacta_raw IS 'stores valacta data files as json, because data is so fuzzy ';


--
-- Name: valacta_data; Type: MATERIALIZED VIEW; Schema: batch; Owner: -
--

CREATE MATERIALIZED VIEW batch.valacta_data AS
 SELECT (COALESCE((valacta_raw.data ->> 'HERD'::text), ''::text))::numeric AS herd,
    (COALESCE((valacta_raw.data ->> 'CONTROL'::text), ''::text))::numeric AS control,
    btrim(COALESCE((valacta_raw.data ->> 'REG'::text), ''::text)) AS reg,
    (COALESCE((valacta_raw.data ->> 'CHAIN'::text), ''::text))::numeric AS chain,
    COALESCE((valacta_raw.data ->> 'NAME'::text), ''::text) AS name,
    (COALESCE((valacta_raw.data ->> 'BIRTH_DATE'::text), ''::text))::date AS birth_date,
    (COALESCE((valacta_raw.data ->> 'STATUS'::text), ''::text))::numeric AS status,
    (COALESCE((valacta_raw.data ->> 'TEST_DATE'::text), ''::text))::date AS test_date,
    (NULLIF((valacta_raw.data ->> 'MILK_AM'::text), ''::text))::numeric AS milk_am,
    (NULLIF((valacta_raw.data ->> 'MILK_PM'::text), ''::text))::numeric AS milk_pm,
    (NULLIF((valacta_raw.data ->> 'MILK_3X'::text), ''::text))::numeric AS milk_3x,
    (COALESCE((valacta_raw.data ->> 'TOTAL_MILK'::text), ''::text))::numeric AS total_milk,
    (COALESCE((valacta_raw.data ->> 'FAT_PER'::text), ''::text))::numeric AS fat_per,
    (COALESCE((valacta_raw.data ->> 'PROT_PER'::text), ''::text))::numeric AS prot_per,
    (NULLIF((valacta_raw.data ->> 'LACTOSE_PER'::text), ''::text))::numeric AS lactose_per,
    (COALESCE((valacta_raw.data ->> 'SSC'::text), ''::text))::numeric AS ssc,
    (COALESCE((valacta_raw.data ->> 'MUN'::text), ''::text))::numeric AS mun,
    (NULLIF((valacta_raw.data ->> 'BHB'::text), ''::text))::numeric AS bhb,
    (COALESCE((valacta_raw.data ->> 'LACT_NU'::text), ''::text))::numeric AS lact_nu,
    (COALESCE((valacta_raw.data ->> 'FRESH'::text), NULL::text))::date AS fresh,
    COALESCE((valacta_raw.data ->> 'EVENT_CODE'::text), ''::text) AS event_code,
    (COALESCE((valacta_raw.data ->> 'LACT_END'::text), NULL::text))::date AS lact_end,
    COALESCE((valacta_raw.data ->> 'LACT_E_COD'::text), ''::text) AS lact_e_cod,
    (COALESCE((valacta_raw.data ->> 'LEFT_HERD'::text), NULL::text))::date AS left_herd,
    COALESCE((valacta_raw.data ->> 'LH_REASON1'::text), ''::text) AS lh_reason1,
    COALESCE((valacta_raw.data ->> 'LH_REASON2'::text), ''::text) AS lh_reason2,
    COALESCE((valacta_raw.data ->> 'M_WEIGHT_S'::text), ''::text) AS m_weight_s,
    COALESCE((valacta_raw.data ->> 'CONDITION1'::text), ''::text) AS condition1,
    COALESCE((valacta_raw.data ->> 'CONDITION2'::text), ''::text) AS condition2,
    COALESCE((valacta_raw.data ->> 'SAMPLE1'::text), ''::text) AS sample1,
    COALESCE((valacta_raw.data ->> 'SAMPLE2'::text), ''::text) AS sample2,
    (COALESCE((valacta_raw.data ->> 'DAYS_IN_MI'::text), ''::text))::numeric AS days_in_mi,
    (COALESCE((valacta_raw.data ->> 'LACT_MILK'::text), ''::text))::numeric AS lact_milk,
    (COALESCE((valacta_raw.data ->> 'LACT_FAT'::text), ''::text))::numeric AS lact_fat,
    (COALESCE((valacta_raw.data ->> 'LACT_PROT'::text), ''::text))::numeric AS lact_prot,
    COALESCE((valacta_raw.data ->> 'BCA_INDICT'::text), ''::text) AS bca_indict,
    (COALESCE((valacta_raw.data ->> 'BCA_MILK'::text), ''::text))::numeric AS bca_milk,
    (COALESCE((valacta_raw.data ->> 'BCA_FAT'::text), ''::text))::numeric AS bca_fat,
    (COALESCE((valacta_raw.data ->> 'BCA_PROT'::text), ''::text))::numeric AS bca_prot,
    (COALESCE((valacta_raw.data ->> 'MILK305'::text), ''::text))::numeric AS milk305,
    (COALESCE((valacta_raw.data ->> 'MILK_REL'::text), ''::text))::numeric AS milk_rel,
    (COALESCE((valacta_raw.data ->> 'FAT305'::text), ''::text))::numeric AS fat305,
    (COALESCE((valacta_raw.data ->> 'FAT_REL'::text), ''::text))::numeric AS fat_rel,
    (COALESCE((valacta_raw.data ->> 'PROT305'::text), ''::text))::numeric AS prot305,
    (COALESCE((valacta_raw.data ->> 'PROT_REL'::text), ''::text))::numeric AS prot_rel,
    (COALESCE((valacta_raw.data ->> 'MILK_DEV'::text), ''::text))::numeric AS milk_dev,
    (COALESCE((valacta_raw.data ->> 'FAT_DEV'::text), ''::text))::numeric AS fat_dev,
    (COALESCE((valacta_raw.data ->> 'PROT_DEV'::text), ''::text))::numeric AS prot_dev,
    (COALESCE((valacta_raw.data ->> 'M_INTV_BRE'::text), ''::text))::numeric AS m_intv_bre,
    (COALESCE((valacta_raw.data ->> 'F_INTV_BRE'::text), ''::text))::numeric AS f_intv_bre,
    (COALESCE((valacta_raw.data ->> 'P_INTV_BRE'::text), ''::text))::numeric AS p_intv_bre,
    (COALESCE((valacta_raw.data ->> 'PEAK_DIM'::text), ''::text))::numeric AS peak_dim,
    (COALESCE((valacta_raw.data ->> 'PEAK_MILK'::text), ''::text))::numeric AS peak_milk,
    (COALESCE((valacta_raw.data ->> 'MILK_STAND'::text), ''::text))::numeric AS milk_stand,
    COALESCE((valacta_raw.data ->> 'FLAG_MILK'::text), ''::text) AS flag_milk,
    (COALESCE((valacta_raw.data ->> 'BREED_DAT1'::text), ''::text))::date AS breed_dat1,
    COALESCE((valacta_raw.data ->> 'SERVICE1'::text), ''::text) AS service1,
    COALESCE((valacta_raw.data ->> 'B_CODE1'::text), ''::text) AS b_code1,
    (COALESCE((valacta_raw.data ->> 'BREED_DAT2'::text), ''::text))::date AS breed_dat2,
    COALESCE((valacta_raw.data ->> 'SERVICE2'::text), ''::text) AS service2,
    COALESCE((valacta_raw.data ->> 'B_CODE2'::text), ''::text) AS b_code2,
    COALESCE((valacta_raw.data ->> 'STOP'::text), ''::text) AS stop,
    COALESCE((valacta_raw.data ->> 'PREGNANT'::text), ''::text) AS pregnant,
    (COALESCE((valacta_raw.data ->> 'LINEAR_SSC'::text), ''::text))::numeric AS linear_ssc,
    (COALESCE((valacta_raw.data ->> 'BODY_COND'::text), ''::text))::numeric AS body_cond,
    (COALESCE((valacta_raw.data ->> 'BODY_WEIGH'::text), ''::text))::numeric AS body_weigh,
    (COALESCE((valacta_raw.data ->> 'PERSISTANC'::text), ''::text))::numeric AS persistanc,
    (COALESCE((valacta_raw.data ->> 'DAYS_TO_FI'::text), ''::text))::numeric AS days_to_fi,
    (COALESCE((valacta_raw.data ->> 'DAYS_TO_L'::text), ''::text))::numeric AS days_to_l,
    (COALESCE((valacta_raw.data ->> 'TIMES_BRED'::text), ''::text))::numeric AS times_bred,
    (COALESCE((valacta_raw.data ->> 'DAYS_DRY'::text), ''::text))::numeric AS days_dry,
    (COALESCE((valacta_raw.data ->> 'COW_WEIGHT'::text), ''::text))::numeric AS cow_weight,
    (COALESCE((valacta_raw.data ->> 'VERIFY'::text), ''::text))::numeric AS verify
   FROM batch.valacta_raw
  WITH NO DATA;


--
-- Name: valacta_data_latest_test_view; Type: VIEW; Schema: batch; Owner: -
--

CREATE VIEW batch.valacta_data_latest_test_view AS
 SELECT valacta_data.herd,
    valacta_data.control,
    valacta_data.reg,
    valacta_data.chain,
    valacta_data.name,
    valacta_data.birth_date,
    valacta_data.status,
    valacta_data.test_date,
    valacta_data.milk_am,
    valacta_data.milk_pm,
    valacta_data.milk_3x,
    valacta_data.total_milk,
    valacta_data.fat_per,
    valacta_data.prot_per,
    valacta_data.lactose_per,
    valacta_data.ssc,
    valacta_data.mun,
    valacta_data.bhb,
    valacta_data.lact_nu,
    valacta_data.fresh,
    valacta_data.event_code,
    valacta_data.lact_end,
    valacta_data.lact_e_cod,
    valacta_data.left_herd,
    valacta_data.lh_reason1,
    valacta_data.lh_reason2,
    valacta_data.m_weight_s,
    valacta_data.condition1,
    valacta_data.condition2,
    valacta_data.sample1,
    valacta_data.sample2,
    valacta_data.days_in_mi,
    valacta_data.lact_milk,
    valacta_data.lact_fat,
    valacta_data.lact_prot,
    valacta_data.bca_indict,
    valacta_data.bca_milk,
    valacta_data.bca_fat,
    valacta_data.bca_prot,
    valacta_data.milk305,
    valacta_data.milk_rel,
    valacta_data.fat305,
    valacta_data.fat_rel,
    valacta_data.prot305,
    valacta_data.prot_rel,
    valacta_data.milk_dev,
    valacta_data.fat_dev,
    valacta_data.prot_dev,
    valacta_data.m_intv_bre,
    valacta_data.f_intv_bre,
    valacta_data.p_intv_bre,
    valacta_data.peak_dim,
    valacta_data.peak_milk,
    valacta_data.milk_stand,
    valacta_data.flag_milk,
    valacta_data.breed_dat1,
    valacta_data.service1,
    valacta_data.b_code1,
    valacta_data.breed_dat2,
    valacta_data.service2,
    valacta_data.b_code2,
    valacta_data.stop,
    valacta_data.pregnant,
    valacta_data.linear_ssc,
    valacta_data.body_cond,
    valacta_data.body_weigh,
    valacta_data.persistanc,
    valacta_data.days_to_fi,
    valacta_data.days_to_l,
    valacta_data.times_bred,
    valacta_data.days_dry,
    valacta_data.cow_weight,
    valacta_data.verify
   FROM batch.valacta_data
  WHERE (valacta_data.test_date = ( SELECT max(valacta_data_1.test_date) AS max
           FROM batch.valacta_data valacta_data_1));


--
-- Name: valacta_group_averages; Type: TABLE; Schema: batch; Owner: -
--

CREATE TABLE batch.valacta_group_averages (
    event_time timestamp without time zone NOT NULL,
    location_id integer NOT NULL,
    avg_fat_per numeric NOT NULL,
    avg_prot_per numeric NOT NULL,
    avg_lactose_per numeric
);


--
-- Name: TABLE valacta_group_averages; Type: COMMENT; Schema: batch; Owner: -
--

COMMENT ON TABLE batch.valacta_group_averages IS 'Valacta component group averages';


--
-- Name: valcata_datam; Type: MATERIALIZED VIEW; Schema: batch; Owner: -
--

CREATE MATERIALIZED VIEW batch.valcata_datam AS
 SELECT (COALESCE((valacta_raw.data ->> 'HERD'::text), ''::text))::numeric AS herd,
    (COALESCE((valacta_raw.data ->> 'CONTROL'::text), ''::text))::numeric AS control,
    COALESCE((valacta_raw.data ->> 'REG'::text), ''::text) AS reg,
    (COALESCE((valacta_raw.data ->> 'CHAIN'::text), ''::text))::numeric AS chain,
    COALESCE((valacta_raw.data ->> 'NAME'::text), ''::text) AS name,
    (COALESCE((valacta_raw.data ->> 'BIRTH_DATE'::text), ''::text))::date AS birth_date,
    (COALESCE((valacta_raw.data ->> 'STATUS'::text), ''::text))::numeric AS status,
    (COALESCE((valacta_raw.data ->> 'TEST_DATE'::text), ''::text))::date AS test_date,
    (COALESCE((valacta_raw.data ->> 'MILK_AM'::text), ''::text))::numeric AS milk_am,
    (COALESCE((valacta_raw.data ->> 'MILK_PM'::text), ''::text))::numeric AS milk_pm,
    (COALESCE((valacta_raw.data ->> 'MILK_3X'::text), ''::text))::numeric AS milk_3x,
    (COALESCE((valacta_raw.data ->> 'TOTAL_MILK'::text), ''::text))::numeric AS total_milk,
    (COALESCE((valacta_raw.data ->> 'FAT_PER'::text), ''::text))::numeric AS fat_per,
    (COALESCE((valacta_raw.data ->> 'PROT_PER'::text), ''::text))::numeric AS prot_per,
    (COALESCE((valacta_raw.data ->> 'SSC'::text), ''::text))::numeric AS ssc,
    (COALESCE((valacta_raw.data ->> 'MUN'::text), ''::text))::numeric AS mun,
    (COALESCE((valacta_raw.data ->> 'LACT_NU'::text), ''::text))::numeric AS lact_nu,
    (COALESCE((valacta_raw.data ->> 'FRESH'::text), NULL::text))::date AS fresh,
    COALESCE((valacta_raw.data ->> 'EVENT_CODE'::text), ''::text) AS event_code,
    (COALESCE((valacta_raw.data ->> 'LACT_END'::text), NULL::text))::date AS lact_end,
    COALESCE((valacta_raw.data ->> 'LACT_E_COD'::text), ''::text) AS lact_e_cod,
    (COALESCE((valacta_raw.data ->> 'LEFT_HERD'::text), NULL::text))::date AS left_herd,
    COALESCE((valacta_raw.data ->> 'LH_REASON1'::text), ''::text) AS lh_reason1,
    COALESCE((valacta_raw.data ->> 'LH_REASON2'::text), ''::text) AS lh_reason2,
    COALESCE((valacta_raw.data ->> 'M_WEIGHT_S'::text), ''::text) AS m_weight_s,
    COALESCE((valacta_raw.data ->> 'CONDITION1'::text), ''::text) AS condition1,
    COALESCE((valacta_raw.data ->> 'CONDITION2'::text), ''::text) AS condition2,
    COALESCE((valacta_raw.data ->> 'SAMPLE1'::text), ''::text) AS sample1,
    COALESCE((valacta_raw.data ->> 'SAMPLE2'::text), ''::text) AS sample2,
    (COALESCE((valacta_raw.data ->> 'DAYS_IN_MI'::text), ''::text))::numeric AS days_in_mi,
    (COALESCE((valacta_raw.data ->> 'LACT_MILK'::text), ''::text))::numeric AS lact_milk,
    (COALESCE((valacta_raw.data ->> 'LACT_FAT'::text), ''::text))::numeric AS lact_fat,
    (COALESCE((valacta_raw.data ->> 'LACT_PROT'::text), ''::text))::numeric AS lact_prot,
    COALESCE((valacta_raw.data ->> 'BCA_INDICT'::text), ''::text) AS bca_indict,
    (COALESCE((valacta_raw.data ->> 'BCA_MILK'::text), ''::text))::numeric AS bca_milk,
    (COALESCE((valacta_raw.data ->> 'BCA_FAT'::text), ''::text))::numeric AS bca_fat,
    (COALESCE((valacta_raw.data ->> 'BCA_PROT'::text), ''::text))::numeric AS bca_prot,
    (COALESCE((valacta_raw.data ->> 'MILK305'::text), ''::text))::numeric AS milk305,
    (COALESCE((valacta_raw.data ->> 'MILK_REL'::text), ''::text))::numeric AS milk_rel,
    (COALESCE((valacta_raw.data ->> 'FAT305'::text), ''::text))::numeric AS fat305,
    (COALESCE((valacta_raw.data ->> 'FAT_REL'::text), ''::text))::numeric AS fat_rel,
    (COALESCE((valacta_raw.data ->> 'PROT305'::text), ''::text))::numeric AS prot305,
    (COALESCE((valacta_raw.data ->> 'PROT_REL'::text), ''::text))::numeric AS prot_rel,
    (COALESCE((valacta_raw.data ->> 'MILK_DEV'::text), ''::text))::numeric AS milk_dev,
    (COALESCE((valacta_raw.data ->> 'FAT_DEV'::text), ''::text))::numeric AS fat_dev,
    (COALESCE((valacta_raw.data ->> 'PROT_DEV'::text), ''::text))::numeric AS prot_dev,
    (COALESCE((valacta_raw.data ->> 'M_INTV_BRE'::text), ''::text))::numeric AS m_intv_bre,
    (COALESCE((valacta_raw.data ->> 'F_INTV_BRE'::text), ''::text))::numeric AS f_intv_bre,
    (COALESCE((valacta_raw.data ->> 'P_INTV_BRE'::text), ''::text))::numeric AS p_intv_bre,
    (COALESCE((valacta_raw.data ->> 'PEAK_DIM'::text), ''::text))::numeric AS peak_dim,
    (COALESCE((valacta_raw.data ->> 'PEAK_MILK'::text), ''::text))::numeric AS peak_milk,
    (COALESCE((valacta_raw.data ->> 'MILK_STAND'::text), ''::text))::numeric AS milk_stand,
    COALESCE((valacta_raw.data ->> 'FLAG_MILK'::text), ''::text) AS flag_milk,
    (COALESCE((valacta_raw.data ->> 'BREED_DAT1'::text), ''::text))::date AS breed_dat1,
    COALESCE((valacta_raw.data ->> 'SERVICE1'::text), ''::text) AS service1,
    COALESCE((valacta_raw.data ->> 'B_CODE1'::text), ''::text) AS b_code1,
    (COALESCE((valacta_raw.data ->> 'BREED_DAT2'::text), ''::text))::date AS breed_dat2,
    COALESCE((valacta_raw.data ->> 'SERVICE2'::text), ''::text) AS service2,
    COALESCE((valacta_raw.data ->> 'B_CODE2'::text), ''::text) AS b_code2,
    COALESCE((valacta_raw.data ->> 'STOP'::text), ''::text) AS stop,
    COALESCE((valacta_raw.data ->> 'PREGNANT'::text), ''::text) AS pregnant,
    (COALESCE((valacta_raw.data ->> 'LINEAR_SSC'::text), ''::text))::numeric AS linear_ssc,
    (COALESCE((valacta_raw.data ->> 'BODY_COND'::text), ''::text))::numeric AS body_cond,
    (COALESCE((valacta_raw.data ->> 'BODY_WEIGH'::text), ''::text))::numeric AS body_weigh,
    (COALESCE((valacta_raw.data ->> 'PERSISTANC'::text), ''::text))::numeric AS persistanc,
    (COALESCE((valacta_raw.data ->> 'DAYS_TO_FI'::text), ''::text))::numeric AS days_to_fi,
    (COALESCE((valacta_raw.data ->> 'DAYS_TO_L'::text), ''::text))::numeric AS days_to_l,
    (COALESCE((valacta_raw.data ->> 'TIMES_BRED'::text), ''::text))::numeric AS times_bred,
    (COALESCE((valacta_raw.data ->> 'DAYS_DRY'::text), ''::text))::numeric AS days_dry,
    (COALESCE((valacta_raw.data ->> 'COW_WEIGHT'::text), ''::text))::numeric AS cow_weight,
    (COALESCE((valacta_raw.data ->> 'VERIFY'::text), ''::text))::numeric AS verify
   FROM batch.valacta_raw
  WITH NO DATA;


--
-- Name: medical_case; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medical_case (
    id integer NOT NULL,
    bovine_id integer NOT NULL,
    open_date date,
    close_date date,
    diagnosis_id integer,
    open_userid character varying(32) NOT NULL,
    close_userid character(32),
    create_time timestamp without time zone,
    update_time timestamp without time zone
);


--
-- Name: TABLE medical_case; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.medical_case IS 'Tracks individual medical cases for bovines.';


--
-- Name: medical_generic_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.medical_generic_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: medical_generic_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.medical_generic_id_seq OWNED BY bovinemanagement.medical_case.id;


--
-- Name: SEQUENCE medical_generic_id_seq; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON SEQUENCE bovinemanagement.medical_generic_id_seq IS 'generic sequence for all medical related items.';


--
-- Name: body_condition_score_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.body_condition_score_event (
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    bovine_id integer NOT NULL,
    score numeric NOT NULL,
    comment character varying,
    userid character(32) NOT NULL
);


--
-- Name: COLUMN body_condition_score_event.score; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.body_condition_score_event.score IS 'body condition score';


--
-- Name: body_condition_score_curr; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.body_condition_score_curr AS
 SELECT DISTINCT ON (body_condition_score_event.bovine_id) body_condition_score_event.bovine_id,
    body_condition_score_event.event_time,
    body_condition_score_event.score,
    body_condition_score_event.comment,
    body_condition_score_event.userid
   FROM bovinemanagement.body_condition_score_event
  ORDER BY body_condition_score_event.bovine_id, body_condition_score_event.event_time DESC;


--
-- Name: VIEW body_condition_score_curr; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.body_condition_score_curr IS 'Current body condition score for all animals dead/alive.';


--
-- Name: bovine_idx_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.bovine_idx_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: bovine_idx_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.bovine_idx_seq OWNED BY bovinemanagement.bovine.id;


--
-- Name: bovineall; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.bovineall AS
 SELECT DISTINCT ON (bovine.id) bovine.id,
    bovine.local_number,
    bovine.full_reg_number,
    bovine.full_name,
    bovine.sire_full_reg_number,
    bovine.dam_full_reg_number,
    locationcurr.event_time AS location_last_event_time,
    locationcurr.location_id,
    locationcurr.name AS location_name,
    locationcurr.dbase_name AS location_name_dbase,
    bovine.birth_date,
    bovine.rfid_number,
    lactationcurr.fresh_date,
    lactationcurr.dry_date,
    eartag_left.eartag_left_name AS eartag_left,
    eartag_right.eartag_right_name AS eartag_right
   FROM ((((bovinemanagement.bovine
     LEFT JOIN bovinemanagement.locationcurr ON ((bovine.id = locationcurr.bovine_id)))
     LEFT JOIN bovinemanagement.lactationcurr ON ((bovine.id = lactationcurr.bovine_id)))
     LEFT JOIN bovinemanagement.eartag_left ON ((eartag_left.bovine_id = bovine.id)))
     LEFT JOIN bovinemanagement.eartag_right ON ((eartag_right.bovine_id = bovine.id)))
  GROUP BY bovine.id, bovine.local_number, locationcurr.event_time, locationcurr.location_id, locationcurr.name, locationcurr.dbase_name, bovine.full_reg_number, bovine.full_name, bovine.sire_full_reg_number, bovine.dam_full_reg_number, bovine.birth_date, bovine.rfid_number, lactationcurr.fresh_date, lactationcurr.dry_date, eartag_left.eartag_left_name, eartag_right.eartag_right_name
  ORDER BY bovine.id;


--
-- Name: bovinecurrall; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.bovinecurrall AS
 SELECT bovine.id,
    bovine.local_number,
    bovine.full_reg_number,
    bovine.full_name,
    bovine.sire_full_reg_number,
    bovine.dam_full_reg_number,
    locationcurr.event_time AS location_last_event_time,
    locationcurr.location_id,
    locationcurr.name AS location_name,
    locationcurr.dbase_name AS location_name_dbase,
    bovine.birth_date,
    bovine.rfid_number,
    lactationcurr.fresh_date,
    lactationcurr.dry_date,
    eartag_left.eartag_left_name AS eartag_left,
    eartag_right.eartag_right_name AS eartag_right,
    bovinemanagement.rfid_purebred(bovine.rfid_number) AS purebred
   FROM ((((bovinemanagement.bovine
     JOIN bovinemanagement.locationcurr ON ((bovine.id = locationcurr.bovine_id)))
     LEFT JOIN bovinemanagement.lactationcurr ON ((bovine.id = lactationcurr.bovine_id)))
     LEFT JOIN bovinemanagement.eartag_left ON ((eartag_left.bovine_id = bovine.id)))
     LEFT JOIN bovinemanagement.eartag_right ON ((eartag_right.bovine_id = bovine.id)))
  WHERE ((bovine.death_date IS NULL) AND (locationcurr.location_on_farm = true))
  GROUP BY bovine.id, bovine.local_number, locationcurr.event_time, locationcurr.location_id, locationcurr.name, locationcurr.dbase_name, bovine.full_reg_number, bovine.full_name, bovine.sire_full_reg_number, bovine.dam_full_reg_number, bovine.birth_date, bovine.rfid_number, lactationcurr.fresh_date, lactationcurr.dry_date, eartag_left.eartag_left_name, eartag_right.eartag_right_name;


--
-- Name: VIEW bovinecurrall; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.bovinecurrall IS 'purebred and non purebred';


--
-- Name: bovinemanagement_generic_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.bovinemanagement_generic_seq
    START WITH 8000
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: SEQUENCE bovinemanagement_generic_seq; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON SEQUENCE bovinemanagement.bovinemanagement_generic_seq IS 'generic sequence for bovinemanagement tables';


--
-- Name: breeding_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.breeding_event (
    id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    estimated_breeding_time timestamp without time zone NOT NULL,
    actual_breeding_time timestamp without time zone,
    estimated_breeding_userid character(32) NOT NULL,
    actual_breeding_userid character(32),
    bovine_id integer NOT NULL,
    semen_straw_id integer,
    semen_straw_choice_userid character(32),
    state_frozen boolean DEFAULT false NOT NULL,
    comment_id integer,
    protocol_uuid uuid
);


--
-- Name: TABLE breeding_event; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.breeding_event IS 'Keeps track of breedings to cows.';


--
-- Name: COLUMN breeding_event.actual_breeding_userid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.breeding_event.actual_breeding_userid IS 'Person who actually did the breeding.';


--
-- Name: COLUMN breeding_event.semen_straw_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.breeding_event.semen_straw_id IS 'semen chosen for breeding.';


--
-- Name: COLUMN breeding_event.semen_straw_choice_userid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.breeding_event.semen_straw_choice_userid IS 'who chose what semen to breed the cow too.';


--
-- Name: COLUMN breeding_event.state_frozen; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.breeding_event.state_frozen IS 'true when breeding slip printed, allows on breeding to be confirmed and no more changes.';


--
-- Name: COLUMN breeding_event.comment_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.breeding_event.comment_id IS 'comments are optional.';


--
-- Name: COLUMN breeding_event.protocol_uuid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.breeding_event.protocol_uuid IS 'used for repro protocols';


--
-- Name: breeding_event_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.breeding_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: breeding_event_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.breeding_event_id_seq OWNED BY bovinemanagement.breeding_event.id;


--
-- Name: protocol_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.protocol_event (
    id integer NOT NULL,
    bovine_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    protocol_type_id integer NOT NULL,
    date_start date NOT NULL,
    protocol_uuid uuid NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE protocol_event; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.protocol_event IS 'actual cows who have had breeding events.';


--
-- Name: COLUMN protocol_event.protocol_type_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.protocol_event.protocol_type_id IS 'type of protocol.';


--
-- Name: COLUMN protocol_event.protocol_uuid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.protocol_event.protocol_uuid IS 'root index for protocol events';


--
-- Name: breeding_protocol_event_id_seq1; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.breeding_protocol_event_id_seq1
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: breeding_protocol_event_id_seq1; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.breeding_protocol_event_id_seq1 OWNED BY bovinemanagement.protocol_event.id;


--
-- Name: protocol_type; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.protocol_type (
    id integer NOT NULL,
    name character varying(64) NOT NULL,
    active boolean DEFAULT true NOT NULL,
    type character varying NOT NULL,
    CONSTRAINT protocol_type CHECK (((type)::text = ANY (ARRAY[('reproductive'::character varying)::text, 'calf'::text, ('medical'::character varying)::text])))
);


--
-- Name: TABLE protocol_type; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.protocol_type IS 'stores different types of breeding protocols.';


--
-- Name: COLUMN protocol_type.type; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.protocol_type.type IS 'medical or reproductive';


--
-- Name: breeding_protocol_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.breeding_protocol_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: breeding_protocol_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.breeding_protocol_id_seq OWNED BY bovinemanagement.protocol_type.id;


--
-- Name: palpate_comment; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.palpate_comment (
    id integer NOT NULL,
    comment character varying(256) NOT NULL
);


--
-- Name: TABLE palpate_comment; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.palpate_comment IS 'Holds different comments for anal palpation.';


--
-- Name: breeding_view; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.breeding_view AS
 SELECT breeding_event.id,
    breeding_event.estimated_breeding_time,
    breeding_event.estimated_breeding_userid,
    breeding_event.actual_breeding_time,
    breeding_event.actual_breeding_userid,
    breeding_event.bovine_id,
    bovine.local_number,
    bovine.full_name,
    sire.full_name AS service_sire_full_name,
    sire.short_name AS service_sire_short_name,
    sire.full_reg_number AS sire_full_reg_number,
    breeding_event.semen_straw_choice_userid,
    sire_semen_code.sexed_semen,
    palpate_comment.comment AS breeding_comment
   FROM (((((bovinemanagement.breeding_event
     LEFT JOIN bovinemanagement.bovine ON ((breeding_event.bovine_id = bovine.id)))
     LEFT JOIN bovinemanagement.semen_straw ON ((breeding_event.semen_straw_id = semen_straw.id)))
     LEFT JOIN bovinemanagement.sire_semen_code ON ((sire_semen_code.semen_code = semen_straw.semen_code)))
     LEFT JOIN bovinemanagement.sire ON ((sire.full_reg_number = sire_semen_code.sire_full_reg_number)))
     LEFT JOIN bovinemanagement.palpate_comment ON ((palpate_comment.id = breeding_event.comment_id)))
  ORDER BY breeding_event.actual_breeding_time DESC;


--
-- Name: calf_milk; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.calf_milk (
    id integer NOT NULL,
    name text NOT NULL
);


--
-- Name: TABLE calf_milk; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.calf_milk IS 'types of calf milk';


--
-- Name: calf_milk_administered; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.calf_milk_administered (
    bovine_id integer NOT NULL,
    calf_milk_id integer NOT NULL,
    dosage numeric NOT NULL,
    dosage_unit character varying DEFAULT 'l'::character varying NOT NULL,
    note character varying,
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    userid character(32),
    update_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone,
    medical_case_id integer,
    scheduled_event_time timestamp without time zone,
    scheduled_userid character(32),
    calendar_event_id text,
    uuid uuid
);


--
-- Name: TABLE calf_milk_administered; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.calf_milk_administered IS 'milk/liquid given to calves manually';


--
-- Name: COLUMN calf_milk_administered.dosage_unit; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calf_milk_administered.dosage_unit IS 'always liters';


--
-- Name: COLUMN calf_milk_administered.medical_case_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calf_milk_administered.medical_case_id IS 'NOT CURRENTLY USED';


--
-- Name: COLUMN calf_milk_administered.scheduled_event_time; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calf_milk_administered.scheduled_event_time IS 'NOT CURRENTLY USED';


--
-- Name: COLUMN calf_milk_administered.scheduled_userid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calf_milk_administered.scheduled_userid IS 'NOT CURRENTLY USED';


--
-- Name: COLUMN calf_milk_administered.calendar_event_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calf_milk_administered.calendar_event_id IS 'NOT CURRENTLY USED';


--
-- Name: COLUMN calf_milk_administered.uuid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calf_milk_administered.uuid IS 'NOT CURRENTLY USED';


--
-- Name: calf_milk_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.calf_milk_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: calf_milk_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.calf_milk_id_seq OWNED BY bovinemanagement.calf_milk.id;


--
-- Name: calf_potential_name; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.calf_potential_name (
    bovine_id integer NOT NULL,
    potential_name character(12) NOT NULL,
    userid character(32) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    id integer NOT NULL
);


--
-- Name: TABLE calf_potential_name; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.calf_potential_name IS 'potential names for a calf';


--
-- Name: calf_potential_name_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.calf_potential_name_id_seq
    START WITH 10000
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: calf_potential_name_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.calf_potential_name_id_seq OWNED BY bovinemanagement.calf_potential_name.id;


--
-- Name: calving_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.calving_event (
    id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    calf_sex character varying(6) NOT NULL,
    alive_or_dead character varying(5) NOT NULL,
    calf_rfid_number numeric,
    lactation_id integer NOT NULL,
    userid character(32) NOT NULL,
    calving_ease integer NOT NULL,
    calf_mass integer,
    calf_bovine_id integer,
    CONSTRAINT calf_sex_check CHECK ((((calf_sex)::text = 'female'::text) OR ((calf_sex)::text = 'male'::text)))
);


--
-- Name: TABLE calving_event; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.calving_event IS 'Tracks any calf born, dead or alive.';


--
-- Name: COLUMN calving_event.calf_sex; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calving_event.calf_sex IS 'male or female';


--
-- Name: COLUMN calving_event.alive_or_dead; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calving_event.alive_or_dead IS 'alive or dead';


--
-- Name: COLUMN calving_event.calf_rfid_number; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calving_event.calf_rfid_number IS 'optional if calf has one (only alive ones will)';


--
-- Name: COLUMN calving_event.lactation_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calving_event.lactation_id IS 'lactation for whichever cow had the calf.';


--
-- Name: COLUMN calving_event.calving_ease; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calving_event.calving_ease IS 'links to calving_event_calving_ease_types';


--
-- Name: COLUMN calving_event.calf_mass; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calving_event.calf_mass IS 'in kilograms';


--
-- Name: COLUMN calving_event.calf_bovine_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.calving_event.calf_bovine_id IS 'if calf was put in bovine table, put id here.';


--
-- Name: calving_event_ease_types; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.calving_event_ease_types (
    id integer NOT NULL,
    name character varying(64) NOT NULL
);


--
-- Name: TABLE calving_event_ease_types; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.calving_event_ease_types IS 'Adlic names for how easy a calf comes out.';


--
-- Name: calving_event_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.calving_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: calving_event_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.calving_event_id_seq OWNED BY bovinemanagement.calving_event.id;


--
-- Name: calving_stats_ease_types_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.calving_stats_ease_types_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: calving_stats_ease_types_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.calving_stats_ease_types_id_seq OWNED BY bovinemanagement.calving_event_ease_types.id;


--
-- Name: embryo_flush; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.embryo_flush (
    id integer NOT NULL,
    donor_dam_full_reg_number character varying(18) NOT NULL,
    flush_date date NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    protocol_uuid uuid
);


--
-- Name: TABLE embryo_flush; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.embryo_flush IS 'main table for when cow is flushed';


--
-- Name: embryo_implant; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.embryo_implant (
    id integer NOT NULL,
    embryo_straw_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone,
    bovine_id integer NOT NULL,
    implanter_userid character(32),
    userid character(32),
    estimated_event_time timestamp without time zone NOT NULL,
    estimated_event_userid character(32) NOT NULL,
    comment_id integer
);


--
-- Name: TABLE embryo_implant; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.embryo_implant IS 'tracks who the embryos were transplanted too.';


--
-- Name: COLUMN embryo_implant.userid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.embryo_implant.userid IS 'employee who recorded the event.';


--
-- Name: COLUMN embryo_implant.estimated_event_time; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.embryo_implant.estimated_event_time IS 'when we think embryo will be placed in recipient.';


--
-- Name: COLUMN embryo_implant.comment_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.embryo_implant.comment_id IS 'embryo Implant Comment';


--
-- Name: embryo_implant_comment; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.embryo_implant_comment (
    id integer NOT NULL,
    comment character varying NOT NULL
);


--
-- Name: embryo_straw; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.embryo_straw (
    id integer NOT NULL,
    embryo_flush_id integer NOT NULL,
    straw_number integer NOT NULL,
    sire_full_reg_number character(18) NOT NULL,
    tank character(1) DEFAULT 'A'::bpchar NOT NULL,
    bin integer,
    quality integer NOT NULL,
    update_time timestamp without time zone DEFAULT '2010-01-01 00:00:00'::timestamp without time zone NOT NULL,
    create_time timestamp without time zone DEFAULT '2010-01-01 00:00:00'::timestamp without time zone NOT NULL,
    cane character(8),
    comment text
);


--
-- Name: COLUMN embryo_straw.straw_number; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.embryo_straw.straw_number IS '1,2,3,4,5, etc. ';


--
-- Name: COLUMN embryo_straw.tank; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.embryo_straw.tank IS 'semen tank letter';


--
-- Name: COLUMN embryo_straw.bin; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.embryo_straw.bin IS '1,2,3,4,5,6,null is unknown bin';


--
-- Name: COLUMN embryo_straw.quality; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.embryo_straw.quality IS '1 is best, 2 usally not frozen';


--
-- Name: COLUMN embryo_straw.cane; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.embryo_straw.cane IS 'stores cane id if given';


--
-- Name: COLUMN embryo_straw.comment; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.embryo_straw.comment IS 'extra info on emybro';


--
-- Name: combined_breeding_embryo_view; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.combined_breeding_embryo_view AS
 SELECT ('breeding|'::text || breeding_event.id) AS id,
    'breeding'::text AS type,
    breeding_event.id AS table_id,
    breeding_event.actual_breeding_time AS event_time,
    breeding_event.actual_breeding_userid AS event_userid,
    palpate_comment.comment AS event_comment,
    bovineall.id AS recipient_bovine_id,
    bovineall.local_number AS recipient_local_number,
    bovineall.full_name AS recipient_full_name,
    bovineall.full_reg_number AS recipient_full_reg_number,
    sire.full_name AS service_sire_full_name,
    sire.short_name AS service_sire_short_name,
    sire.full_reg_number AS service_sire_full_reg_number,
    sire_semen_code.sexed_semen,
    bovineall.id AS donor_dam_bovine_id,
    bovineall.local_number AS donor_dam_local_number,
    bovineall.full_name AS donor_dam_full_name,
    bovineall.full_reg_number AS donor_dam_full_reg_number,
    bovinemanagement.round_to_nearest_date((breeding_event.actual_breeding_time + '280 days 12:00:00'::interval)) AS calculated_potential_due_date
   FROM (((((bovinemanagement.breeding_event
     LEFT JOIN bovinemanagement.bovineall ON ((breeding_event.bovine_id = bovineall.id)))
     LEFT JOIN bovinemanagement.semen_straw ON ((breeding_event.semen_straw_id = semen_straw.id)))
     LEFT JOIN bovinemanagement.sire_semen_code ON ((sire_semen_code.semen_code = semen_straw.semen_code)))
     LEFT JOIN bovinemanagement.sire ON ((sire.full_reg_number = sire_semen_code.sire_full_reg_number)))
     LEFT JOIN bovinemanagement.palpate_comment ON ((palpate_comment.id = breeding_event.comment_id)))
UNION
 SELECT ('embryo7day|'::text || embryo_implant.id) AS id,
    'embryo7day'::text AS type,
    embryo_implant.id AS table_id,
    embryo_implant.event_time,
    embryo_implant.implanter_userid AS event_userid,
    embryo_implant_comment.comment AS event_comment,
    embryo_implant.bovine_id AS recipient_bovine_id,
    bovineall.local_number AS recipient_local_number,
    bovineall.full_name AS recipient_full_name,
    bovineall.full_reg_number AS recipient_full_reg_number,
    sire.full_name AS service_sire_full_name,
    sire.short_name AS service_sire_short_name,
    sire.full_reg_number AS service_sire_full_reg_number,
    false AS sexed_semen,
    bovineall2.id AS donor_dam_bovine_id,
    bovineall2.local_number AS donor_dam_local_number,
    bovineall2.full_name AS donor_dam_full_name,
    embryo_flush.donor_dam_full_reg_number,
    bovinemanagement.round_to_nearest_date((embryo_implant.event_time + '273 days 12:00:00'::interval)) AS calculated_potential_due_date
   FROM ((((((bovinemanagement.embryo_implant
     LEFT JOIN bovinemanagement.bovineall ON ((embryo_implant.bovine_id = bovineall.id)))
     LEFT JOIN bovinemanagement.embryo_straw ON ((embryo_implant.embryo_straw_id = embryo_straw.id)))
     LEFT JOIN bovinemanagement.embryo_flush ON ((embryo_straw.embryo_flush_id = embryo_flush.id)))
     LEFT JOIN bovinemanagement.embryo_implant_comment ON ((embryo_implant_comment.id = embryo_implant.comment_id)))
     LEFT JOIN bovinemanagement.sire ON ((sire.full_reg_number = embryo_straw.sire_full_reg_number)))
     LEFT JOIN bovinemanagement.bovineall bovineall2 ON (((embryo_flush.donor_dam_full_reg_number)::text = (bovineall2.full_reg_number)::text)))
  ORDER BY 3;


--
-- Name: VIEW combined_breeding_embryo_view; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.combined_breeding_embryo_view IS 'combines breedings and embryo implants. makes preg checks etc., easier';


--
-- Name: comment_custom; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.comment_custom (
    id integer NOT NULL,
    bovine_id integer NOT NULL,
    create_time timestamp without time zone,
    update_time timestamp without time zone,
    comment text NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    type character(32) DEFAULT 'reproductive'::bpchar NOT NULL
);


--
-- Name: TABLE comment_custom; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.comment_custom IS 'Reproductive, trait and other misc comments.';


--
-- Name: COLUMN comment_custom.type; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.comment_custom.type IS 'type';


--
-- Name: comment_reproductive_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.comment_reproductive_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: comment_reproductive_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.comment_reproductive_id_seq OWNED BY bovinemanagement.comment_custom.id;


--
-- Name: cull_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.cull_event (
    bovine_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    reason character varying NOT NULL,
    comment text NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    id integer NOT NULL,
    location_event_id integer NOT NULL
);


--
-- Name: TABLE cull_event; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.cull_event IS 'tracks cows who left herd';


--
-- Name: COLUMN cull_event.location_event_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.cull_event.location_event_id IS 'needed to do proper delete';


--
-- Name: cull_event_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.cull_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: cull_event_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.cull_event_id_seq OWNED BY bovinemanagement.cull_event.id;


--
-- Name: cull_list_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.cull_list_event (
    id integer NOT NULL,
    bovine_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    cull_status character(12) NOT NULL,
    userid character(32) NOT NULL,
    CONSTRAINT cull_status_check CHECK ((cull_status = ANY (ARRAY['do_not_cull'::bpchar, 'potential_cull'::bpchar, 'cull'::bpchar])))
);


--
-- Name: TABLE cull_list_event; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.cull_list_event IS 'list of cows to potentially be culled.,';


--
-- Name: cull_list_event_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.cull_list_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: cull_list_event_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.cull_list_event_id_seq OWNED BY bovinemanagement.cull_list_event.id;


--
-- Name: kamar_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.kamar_event (
    id integer NOT NULL,
    bovine_id integer NOT NULL,
    kamar_event_type character(32) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    estrus_id integer,
    CONSTRAINT kamar_event_type_check_updated CHECK ((kamar_event_type = ANY (ARRAY['on'::bpchar, 'removed'::bpchar, 'lost'::bpchar, 'red'::bpchar, 'partialred'::bpchar, 'verifiedwhite'::bpchar])))
);


--
-- Name: TABLE kamar_event; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.kamar_event IS 'Keeps track of Kamar heat events for cows.';


--
-- Name: COLUMN kamar_event.estrus_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.kamar_event.estrus_id IS 'when kamar is red it is a heat event.';


--
-- Name: kamar_event_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.kamar_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: kamar_event_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.kamar_event_id_seq OWNED BY bovinemanagement.kamar_event.id;


--
-- Name: dehorn_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.dehorn_event (
    id integer DEFAULT nextval('bovinemanagement.kamar_event_id_seq'::regclass) NOT NULL,
    bovine_id integer NOT NULL,
    dehorn_event_type character(32) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    CONSTRAINT dehorn_type CHECK (((dehorn_event_type)::text = ANY (ARRAY['burned'::text, 'gouged'::text, 'sawed'::text, 'polled'::text, 'redo'::text])))
);


--
-- Name: dnatest_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.dnatest_event (
    id integer DEFAULT nextval('bovinemanagement.bovinemanagement_generic_seq'::regclass) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    bovine_id integer NOT NULL,
    dnatest_type character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: drycurr; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.drycurr AS
 WITH temp AS (
         SELECT bovinecurr.id,
            bovinecurr.local_number,
            ( SELECT max(lactation.dry_date) AS max
                   FROM bovinemanagement.lactation
                  WHERE (lactation.bovine_id = bovinecurr.id)) AS dry_date
           FROM bovinemanagement.bovinecurr
          WHERE ((bovinecurr.fresh_date IS NULL) AND (bovinecurr.dry_date IS NULL))
        )
 SELECT temp.id,
    temp.local_number,
    temp.dry_date
   FROM temp
  WHERE (temp.dry_date IS NOT NULL);


--
-- Name: VIEW drycurr; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.drycurr IS 'list all the cows who are currently dry. This should maybe be incorporated into bovinecurr where state would be null for fresh_date and true for dry_date?';


--
-- Name: dryoff_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.dryoff_event (
    event_time timestamp without time zone NOT NULL,
    lactation_id integer NOT NULL,
    med_administered_a1 integer NOT NULL,
    med_administered_a2 integer NOT NULL,
    med_administered_a3 integer NOT NULL,
    med_administered_a4 integer NOT NULL,
    med_administered_b1 integer NOT NULL,
    med_administered_b2 integer NOT NULL,
    med_administered_b3 integer NOT NULL,
    med_administered_b4 integer NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE dryoff_event; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.dryoff_event IS 'tracks drying off';


--
-- Name: dryoff_event_med_administered_b2_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.dryoff_event_med_administered_b2_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: dryoff_event_med_administered_b2_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.dryoff_event_med_administered_b2_seq OWNED BY bovinemanagement.dryoff_event.med_administered_b2;


--
-- Name: dryoff_event_med_administered_b3_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.dryoff_event_med_administered_b3_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: dryoff_event_med_administered_b3_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.dryoff_event_med_administered_b3_seq OWNED BY bovinemanagement.dryoff_event.med_administered_b3;


--
-- Name: dryoff_event_med_administered_b4_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.dryoff_event_med_administered_b4_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: dryoff_event_med_administered_b4_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.dryoff_event_med_administered_b4_seq OWNED BY bovinemanagement.dryoff_event.med_administered_b4;


--
-- Name: eartag_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.eartag_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: eartag_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.eartag_id_seq OWNED BY bovinemanagement.eartag.id;


--
-- Name: eartag_valid; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.eartag_valid (
    rfid_range numrange NOT NULL,
    local_num_sig_digits integer DEFAULT 4 NOT NULL,
    local_num_offset integer DEFAULT 0 NOT NULL,
    event_time timestamp without time zone NOT NULL,
    purebred boolean NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE eartag_valid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.eartag_valid IS 'lists range of valid eartags
';


--
-- Name: COLUMN eartag_valid.rfid_range; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.eartag_valid.rfid_range IS '16 digit RFID range of valid ear tag numbers';


--
-- Name: COLUMN eartag_valid.local_num_sig_digits; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.eartag_valid.local_num_sig_digits IS 'usually the last 3 to 5 digits are used as local id';


--
-- Name: COLUMN eartag_valid.local_num_offset; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.eartag_valid.local_num_offset IS 'difference in full RFID number and significant local numbers';


--
-- Name: COLUMN eartag_valid.purebred; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.eartag_valid.purebred IS 'can these eartags be used for purebred registration?';


--
-- Name: embryo_flush_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.embryo_flush_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: embryo_flush_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.embryo_flush_id_seq OWNED BY bovinemanagement.embryo_flush.id;


--
-- Name: embryo_implant_comment_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.embryo_implant_comment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: embryo_implant_comment_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.embryo_implant_comment_id_seq OWNED BY bovinemanagement.embryo_implant_comment.id;


--
-- Name: embryo_recipient_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.embryo_recipient_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: embryo_recipient_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.embryo_recipient_id_seq OWNED BY bovinemanagement.embryo_implant.id;


--
-- Name: embryo_straw_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.embryo_straw_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: embryo_straw_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.embryo_straw_id_seq OWNED BY bovinemanagement.embryo_straw.id;


--
-- Name: embryo_view; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.embryo_view AS
 SELECT ('embryo7day|'::text || embryo_implant.id) AS id,
    'embryo7day'::text AS type,
    embryo_implant.id AS table_id,
    embryo_implant.event_time,
    embryo_implant.implanter_userid AS event_userid,
    embryo_implant_comment.comment AS event_comment,
    embryo_implant.bovine_id AS recipient_bovine_id,
    bovineall.local_number AS recipient_local_number,
    bovineall.full_name AS recipient_full_name,
    bovineall.full_reg_number AS recipient_full_reg_number,
    sire.full_name AS service_sire_full_name,
    sire.short_name AS service_sire_short_name,
    sire.full_reg_number AS service_sire_full_reg_number,
    false AS sexed_semen,
    bovineall2.id AS donor_dam_bovine_id,
    bovineall2.local_number AS donor_dam_local_number,
    bovineall2.full_name AS donor_dam_full_name,
    embryo_flush.donor_dam_full_reg_number,
    bovinemanagement.round_to_nearest_date((embryo_implant.event_time + '273 days 12:00:00'::interval)) AS calculated_potential_due_date
   FROM ((((((bovinemanagement.embryo_implant
     LEFT JOIN bovinemanagement.bovineall ON ((embryo_implant.bovine_id = bovineall.id)))
     LEFT JOIN bovinemanagement.embryo_straw ON ((embryo_implant.embryo_straw_id = embryo_straw.id)))
     LEFT JOIN bovinemanagement.embryo_flush ON ((embryo_straw.embryo_flush_id = embryo_flush.id)))
     LEFT JOIN bovinemanagement.embryo_implant_comment ON ((embryo_implant_comment.id = embryo_implant.comment_id)))
     LEFT JOIN bovinemanagement.sire ON ((sire.full_reg_number = embryo_straw.sire_full_reg_number)))
     LEFT JOIN bovinemanagement.bovineall bovineall2 ON (((embryo_flush.donor_dam_full_reg_number)::text = (bovineall2.full_reg_number)::text)))
  ORDER BY embryo_implant.id;


--
-- Name: estrus_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.estrus_event (
    id integer NOT NULL,
    update_time timestamp without time zone NOT NULL,
    estrus_type_id integer NOT NULL,
    bovine_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    note character varying(255),
    userid character(32) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    kamar_event_id integer
);


--
-- Name: TABLE estrus_event; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.estrus_event IS 'Lists of different observables that happen during the estrus cycle.';


--
-- Name: COLUMN estrus_event.id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.estrus_event.id IS 'id.';


--
-- Name: COLUMN estrus_event.estrus_type_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.estrus_event.estrus_type_id IS 'foreign key.';


--
-- Name: COLUMN estrus_event.bovine_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.estrus_event.bovine_id IS 'foreign key.';


--
-- Name: COLUMN estrus_event.kamar_event_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.estrus_event.kamar_event_id IS 'points to kamar_event table';


--
-- Name: estrus_type; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.estrus_type (
    id integer NOT NULL,
    name text NOT NULL,
    breeding_rating integer NOT NULL,
    display_order integer NOT NULL
);


--
-- Name: TABLE estrus_type; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.estrus_type IS 'Lists of different observables that happen during the estrus cycle.';


--
-- Name: COLUMN estrus_type.id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.estrus_type.id IS 'id.';


--
-- Name: COLUMN estrus_type.name; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.estrus_type.name IS 'name of estrus events.';


--
-- Name: COLUMN estrus_type.breeding_rating; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.estrus_type.breeding_rating IS '0 to 100 value to rate importance for breeding.';


--
-- Name: COLUMN estrus_type.display_order; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.estrus_type.display_order IS 'list of integers used to show order list is displayed in.';


--
-- Name: estrus_event_breeding_rating_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.estrus_event_breeding_rating_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: estrus_event_breeding_rating_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.estrus_event_breeding_rating_seq OWNED BY bovinemanagement.estrus_type.breeding_rating;


--
-- Name: estrus_event_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.estrus_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: estrus_event_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.estrus_event_id_seq OWNED BY bovinemanagement.estrus_event.id;


--
-- Name: estrus_event_name_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.estrus_event_name_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: estrus_event_name_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.estrus_event_name_seq OWNED BY bovinemanagement.estrus_type.name;


--
-- Name: estrus_type_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.estrus_type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: estrus_type_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.estrus_type_id_seq OWNED BY bovinemanagement.estrus_type.id;


--
-- Name: foot_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.foot_event (
    id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    bovine_id integer NOT NULL,
    foot_type_id integer NOT NULL,
    comment character varying,
    userid character(32) NOT NULL,
    foot character varying(15)
);


--
-- Name: COLUMN foot_event.comment; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.foot_event.comment IS 'optional';


--
-- Name: foot_diagnosis_event_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.foot_diagnosis_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: foot_diagnosis_event_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.foot_diagnosis_event_id_seq OWNED BY bovinemanagement.foot_event.id;


--
-- Name: foot_type; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.foot_type (
    id integer NOT NULL,
    name character varying NOT NULL
);


--
-- Name: foot_type_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.foot_type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: foot_type_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.foot_type_id_seq OWNED BY bovinemanagement.foot_type.id;


--
-- Name: footcurr; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.footcurr AS
( SELECT DISTINCT ON (foot_event.bovine_id) bovine.local_number,
    bovine.full_name,
    foot_event.bovine_id,
    foot_event.id AS foot_event_id,
    foot_event.event_time,
    foot_event.foot,
    foot_type.name,
    foot_type.id AS foot_type_id,
    foot_event.userid,
    foot_event.comment
   FROM ((bovinemanagement.foot_event
     LEFT JOIN bovinemanagement.bovine ON ((bovine.id = foot_event.bovine_id)))
     LEFT JOIN bovinemanagement.foot_type ON ((foot_type.id = foot_event.foot_type_id)))
  WHERE (((foot_event.foot)::text = 'rear right'::text) OR ((foot_event.foot)::text = 'general'::text))
  ORDER BY foot_event.bovine_id, foot_event.event_time DESC)
UNION
( SELECT DISTINCT ON (foot_event.bovine_id) bovine.local_number,
    bovine.full_name,
    foot_event.bovine_id,
    foot_event.id AS foot_event_id,
    foot_event.event_time,
    foot_event.foot,
    foot_type.name,
    foot_type.id AS foot_type_id,
    foot_event.userid,
    foot_event.comment
   FROM ((bovinemanagement.foot_event
     LEFT JOIN bovinemanagement.bovine ON ((bovine.id = foot_event.bovine_id)))
     LEFT JOIN bovinemanagement.foot_type ON ((foot_type.id = foot_event.foot_type_id)))
  WHERE (((foot_event.foot)::text = 'rear left'::text) OR ((foot_event.foot)::text = 'general'::text))
  ORDER BY foot_event.bovine_id, foot_event.event_time DESC)
UNION
( SELECT DISTINCT ON (foot_event.bovine_id) bovine.local_number,
    bovine.full_name,
    foot_event.bovine_id,
    foot_event.id AS foot_event_id,
    foot_event.event_time,
    foot_event.foot,
    foot_type.name,
    foot_type.id AS foot_type_id,
    foot_event.userid,
    foot_event.comment
   FROM ((bovinemanagement.foot_event
     LEFT JOIN bovinemanagement.bovine ON ((bovine.id = foot_event.bovine_id)))
     LEFT JOIN bovinemanagement.foot_type ON ((foot_type.id = foot_event.foot_type_id)))
  WHERE (((foot_event.foot)::text = 'front right'::text) OR ((foot_event.foot)::text = 'general'::text))
  ORDER BY foot_event.bovine_id, foot_event.event_time DESC)
UNION
( SELECT DISTINCT ON (foot_event.bovine_id) bovine.local_number,
    bovine.full_name,
    foot_event.bovine_id,
    foot_event.id AS foot_event_id,
    foot_event.event_time,
    foot_event.foot,
    foot_type.name,
    foot_type.id AS foot_type_id,
    foot_event.userid,
    foot_event.comment
   FROM ((bovinemanagement.foot_event
     LEFT JOIN bovinemanagement.bovine ON ((bovine.id = foot_event.bovine_id)))
     LEFT JOIN bovinemanagement.foot_type ON ((foot_type.id = foot_event.foot_type_id)))
  WHERE (((foot_event.foot)::text = 'front left'::text) OR ((foot_event.foot)::text = 'general'::text))
  ORDER BY foot_event.bovine_id, foot_event.event_time DESC)
  ORDER BY 3, 6, 5 DESC;


--
-- Name: herdgenetics; Type: MATERIALIZED VIEW; Schema: bovinemanagement; Owner: -
--

CREATE MATERIALIZED VIEW bovinemanagement.herdgenetics AS
 SELECT bovinecurr.id,
    bovinecurr.local_number,
    aggregate_view_curr.lpi,
    aggregate_view_curr.prodoll,
    aggregate_view_curr.geno_test,
    aggregate_view_curr.score,
    bovinecurr.full_name,
    bovinecurr.full_reg_number,
    bovinecurr.birth_date,
    batch.prodoll_birthyear_quintile_rank(bovinecurr.id, (date_part('year'::text, bovinecurr.birth_date))::integer) AS prodoll_birthyear_quintile_rank
   FROM (bovinemanagement.bovinecurr
     LEFT JOIN batch.aggregate_view_curr ON (((aggregate_view_curr.full_reg_number)::text = (bovinecurr.full_reg_number)::text)))
  ORDER BY aggregate_view_curr.prodoll DESC NULLS LAST
  WITH NO DATA;


--
-- Name: kamar_event_curr; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.kamar_event_curr AS
 WITH temp AS (
         SELECT d1.id,
            d1.bovine_id,
            d1.event_time,
            d1.kamar_event_type,
            d1.userid,
            bovinecurr.local_number,
            bovinecurr.full_name,
            bovinecurr.location_name
           FROM (bovinemanagement.kamar_event d1
             LEFT JOIN bovinemanagement.bovinecurr ON ((bovinecurr.id = d1.bovine_id)))
          WHERE ((d1.event_time IN ( SELECT max(d2.event_time) AS max
                   FROM bovinemanagement.kamar_event d2
                  WHERE (d2.bovine_id = d1.bovine_id))) AND ((d1.kamar_event_type <> 'lost'::bpchar) OR ((d1.kamar_event_type = 'lost'::bpchar) AND (d1.event_time > (now() - '96:00:00'::interval)))) AND ((d1.kamar_event_type <> 'removed'::bpchar) OR ((d1.kamar_event_type = 'removed'::bpchar) AND (d1.event_time > (now() - '24:00:00'::interval)))))
        )
 SELECT temp.id,
    temp.bovine_id,
    temp.event_time,
    temp.kamar_event_type,
    temp.userid,
    temp.local_number,
    temp.full_name,
    temp.location_name
   FROM temp
  WHERE (temp.local_number IS NOT NULL)
  ORDER BY temp.local_number, temp.event_time;


--
-- Name: VIEW kamar_event_curr; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.kamar_event_curr IS 'lost kamars are only shown for last 4 days, removed are only shown for last 24 hours ';


--
-- Name: lactation_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.lactation_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: lactation_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.lactation_id_seq OWNED BY bovinemanagement.lactation.id;


--
-- Name: location_event_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.location_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: location_event_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.location_event_id_seq OWNED BY bovinemanagement.location_event.id;


--
-- Name: location_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.location_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: location_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.location_id_seq OWNED BY bovinemanagement.location.id;


--
-- Name: location_sort; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.location_sort (
    id integer NOT NULL,
    bovine_id integer NOT NULL,
    estimated_sort_time timestamp without time zone NOT NULL,
    actual_sort_time timestamp without time zone,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE location_sort; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.location_sort IS 'tracks sort gate';


--
-- Name: location_sort_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.location_sort_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: location_sort_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.location_sort_id_seq OWNED BY bovinemanagement.location_sort.id;


--
-- Name: location_stats; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.location_stats AS
 SELECT bovinecurr.location_id,
    bovinecurr.location_name,
    count(DISTINCT bovinecurr.id) AS location_total,
    avg(((now())::date - (bovinecurr.fresh_date)::date)) AS dim
   FROM bovinemanagement.bovinecurr
  GROUP BY bovinecurr.location_name, bovinecurr.location_id;


--
-- Name: location_urban_feedstall; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.location_urban_feedstall (
    location_id integer NOT NULL,
    feedstall_id character(7) NOT NULL
);


--
-- Name: TABLE location_urban_feedstall; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.location_urban_feedstall IS 'links pen location id to urban feedstall id';


--
-- Name: medical_action; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medical_action (
    bovine_id integer NOT NULL,
    medical_action_type_id integer NOT NULL,
    comment text,
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    userid character(32),
    update_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone,
    medical_case_id integer,
    scheduled_event_time timestamp without time zone,
    scheduled_userid character(32),
    calendar_event_id text,
    protocol_uuid uuid
);


--
-- Name: medical_action_type; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medical_action_type (
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    action character varying(64) NOT NULL
);


--
-- Name: medical_comment; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medical_comment (
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    bovine_id integer NOT NULL,
    medical_case_id integer,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment character varying NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE medical_comment; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.medical_comment IS '1';


--
-- Name: COLUMN medical_comment.medical_case_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medical_comment.medical_case_id IS 'optional';


--
-- Name: medical_diagnosis; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medical_diagnosis (
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    bovine_id integer NOT NULL,
    medical_case_id integer,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    diagnosis_type_id integer NOT NULL,
    userid character(32)
);


--
-- Name: medical_diagnosis_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.medical_diagnosis_id_seq
    START WITH 10
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: medical_diagnosis_type; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medical_diagnosis_type (
    id integer DEFAULT nextval('bovinemanagement.medical_diagnosis_id_seq'::regclass) NOT NULL,
    diagnosis character varying(64),
    reproductive_related boolean DEFAULT false NOT NULL
);


--
-- Name: TABLE medical_diagnosis_type; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.medical_diagnosis_type IS 'list of medical diagnosis''s';


--
-- Name: medical_ketone; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medical_ketone (
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    bovine_id integer NOT NULL,
    medical_case_id integer,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    ketone numeric NOT NULL
);


--
-- Name: TABLE medical_ketone; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.medical_ketone IS 'stores on farm ketone test results';


--
-- Name: COLUMN medical_ketone.medical_case_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medical_ketone.medical_case_id IS 'optional';


--
-- Name: COLUMN medical_ketone.ketone; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medical_ketone.ketone IS 'units of mmol/liter';


--
-- Name: medical_magnet; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medical_magnet (
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    bovine_id integer NOT NULL,
    medical_case_id integer,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    magnet boolean DEFAULT false NOT NULL
);


--
-- Name: medical_temperature; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medical_temperature (
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    bovine_id integer NOT NULL,
    medical_case_id integer,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    temperature numeric NOT NULL
);


--
-- Name: TABLE medical_temperature; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.medical_temperature IS 'tracks animal temperature';


--
-- Name: COLUMN medical_temperature.medical_case_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medical_temperature.medical_case_id IS 'optional';


--
-- Name: COLUMN medical_temperature.temperature; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medical_temperature.temperature IS 'in Celcius';


--
-- Name: medicine; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medicine (
    created_time timestamp with time zone DEFAULT now() NOT NULL,
    last_update timestamp with time zone DEFAULT now() NOT NULL,
    trade_name character varying NOT NULL,
    latin_name character varying NOT NULL,
    official_instructions text NOT NULL,
    local_instructions text NOT NULL,
    id integer NOT NULL,
    medicine_class character varying(64) NOT NULL,
    milk_withholding integer NOT NULL,
    beef_withholding integer NOT NULL,
    milk_post_calving_withholding integer NOT NULL,
    default_dosage numeric NOT NULL,
    default_dosage_unit character varying NOT NULL,
    for_mastitis boolean NOT NULL,
    default_body_location character varying,
    for_dry_treatment boolean DEFAULT false NOT NULL
);


--
-- Name: TABLE medicine; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.medicine IS 'Table to keep track of different drugs/vaccines/medicines used to treat bovines.';


--
-- Name: COLUMN medicine.milk_withholding; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine.milk_withholding IS 'units in hours';


--
-- Name: COLUMN medicine.beef_withholding; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine.beef_withholding IS 'units in days';


--
-- Name: COLUMN medicine.milk_post_calving_withholding; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine.milk_post_calving_withholding IS 'units in hours, only for post calving hours, general milk withholding overules';


--
-- Name: COLUMN medicine.default_dosage; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine.default_dosage IS 'numeric part of dosage';


--
-- Name: COLUMN medicine.default_dosage_unit; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine.default_dosage_unit IS 'ml,g,kg,etc.';


--
-- Name: COLUMN medicine.default_body_location; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine.default_body_location IS 'default place on/in body where medicine should be used.';


--
-- Name: COLUMN medicine.for_dry_treatment; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine.for_dry_treatment IS 'medicines used for dry treatment';


--
-- Name: medicine_administered; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.medicine_administered (
    bovine_id integer NOT NULL,
    medicine_index integer NOT NULL,
    dosage numeric NOT NULL,
    dosage_unit character varying NOT NULL,
    location character varying(64) NOT NULL,
    note character varying,
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    userid character(32),
    update_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone,
    medical_case_id integer,
    scheduled_event_time timestamp without time zone,
    scheduled_userid character(32),
    calendar_event_id text,
    uuid uuid,
    protocol_uuid uuid,
    CONSTRAINT location4 CHECK (((location)::bpchar = ANY (ARRAY['oral'::bpchar, 'subcutaneous'::bpchar, 'intramuscular'::bpchar, 'intravenous'::bpchar, 'intraperitoneal'::bpchar, 'front left teat canal'::bpchar, 'front right teat canal'::bpchar, 'rear left teat canal'::bpchar, 'rear right teat canal'::bpchar, 'intravaginal'::bpchar, 'skin'::bpchar])))
);


--
-- Name: TABLE medicine_administered; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.medicine_administered IS 'Keeps track when a bovine is treated with some type of medicine.';


--
-- Name: COLUMN medicine_administered.userid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine_administered.userid IS 'can be null for scheduled medicine.';


--
-- Name: COLUMN medicine_administered.event_time; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine_administered.event_time IS 'can be null for scheduled medicine.';


--
-- Name: COLUMN medicine_administered.medical_case_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine_administered.medical_case_id IS 'not null when associated with ongoing medical case.';


--
-- Name: COLUMN medicine_administered.scheduled_event_time; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine_administered.scheduled_event_time IS 'when the drug is scheduled';


--
-- Name: COLUMN medicine_administered.scheduled_userid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine_administered.scheduled_userid IS 'who scheduled the medicine (if any).';


--
-- Name: COLUMN medicine_administered.calendar_event_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine_administered.calendar_event_id IS 'google calendar event unique id.';


--
-- Name: COLUMN medicine_administered.uuid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine_administered.uuid IS 'used for medicines scheduled as transaction';


--
-- Name: COLUMN medicine_administered.protocol_uuid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.medicine_administered.protocol_uuid IS 'used for protocols';


--
-- Name: vet_to_check_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.vet_to_check_event (
    id integer DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass) NOT NULL,
    bovine_id integer NOT NULL,
    medical_case_id integer,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment_entered character varying NOT NULL,
    userid_entered character(32) NOT NULL,
    comment_completed character varying,
    userid_completed character(32),
    event_time_completed timestamp without time zone
);


--
-- Name: COLUMN vet_to_check_event.userid_entered; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.vet_to_check_event.userid_entered IS 'who initiated the vet check';


--
-- Name: COLUMN vet_to_check_event.userid_completed; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.vet_to_check_event.userid_completed IS 'who completed the vet check';


--
-- Name: medical_summary; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.medical_summary AS
 SELECT medical_temperature.id,
    'medical_temperature'::text AS tablename,
    medical_temperature.medical_case_id,
    medical_temperature.bovine_id,
    'Temperature'::text AS type,
    medical_temperature.event_time,
    medical_temperature.update_time,
    (medical_temperature.temperature || chr(8451)) AS text,
    medical_temperature.userid
   FROM bovinemanagement.medical_temperature
UNION
 SELECT medical_comment.id,
    'medical_comment'::text AS tablename,
    medical_comment.medical_case_id,
    medical_comment.bovine_id,
    'Comment'::text AS type,
    medical_comment.event_time,
    medical_comment.update_time,
    medical_comment.comment AS text,
    medical_comment.userid
   FROM bovinemanagement.medical_comment
UNION
 SELECT medicine_administered.id,
    'medicine_administered'::text AS tablename,
    medicine_administered.medical_case_id,
    medicine_administered.bovine_id,
    'Scheduled Meds'::text AS type,
    medicine_administered.scheduled_event_time AS event_time,
    medicine_administered.update_time,
    ((((((medicine.trade_name)::text || ' '::text) || medicine_administered.dosage) || (medicine_administered.dosage_unit)::text) || ' @ '::text) || (medicine_administered.location)::text) AS text,
    medicine_administered.scheduled_userid AS userid
   FROM (bovinemanagement.medicine_administered
     LEFT JOIN bovinemanagement.medicine ON ((medicine_administered.medicine_index = medicine.id)))
  WHERE (medicine_administered.event_time IS NULL)
UNION
 SELECT medicine_administered.id,
    'medicine_administered'::text AS tablename,
    medicine_administered.medical_case_id,
    medicine_administered.bovine_id,
    'Meds'::text AS type,
    medicine_administered.event_time,
    medicine_administered.update_time,
    ((((((medicine.trade_name)::text || ' '::text) || medicine_administered.dosage) || (medicine_administered.dosage_unit)::text) || ' @ '::text) || (medicine_administered.location)::text) AS text,
    medicine_administered.userid
   FROM (bovinemanagement.medicine_administered
     LEFT JOIN bovinemanagement.medicine ON ((medicine_administered.medicine_index = medicine.id)))
  WHERE (medicine_administered.event_time IS NOT NULL)
UNION
 SELECT medical_ketone.id,
    'medical_ketone'::text AS tablename,
    medical_ketone.medical_case_id,
    medical_ketone.bovine_id,
    'Ketone Test'::text AS type,
    medical_ketone.event_time,
    medical_ketone.update_time,
    (medical_ketone.ketone || ' mmol/liter'::text) AS text,
    medical_ketone.userid
   FROM bovinemanagement.medical_ketone
UNION
 SELECT medical_action.id,
    'medical_action'::text AS tablename,
    medical_action.medical_case_id,
    medical_action.bovine_id,
    'Scheduled Action'::text AS type,
    medical_action.scheduled_event_time AS event_time,
    medical_action.update_time,
    (medical_action_type.action)::text AS text,
    medical_action.scheduled_userid AS userid
   FROM (bovinemanagement.medical_action
     LEFT JOIN bovinemanagement.medical_action_type ON ((medical_action.medical_action_type_id = medical_action_type.id)))
  WHERE (medical_action.event_time IS NULL)
UNION
 SELECT medical_action.id,
    'medical_action'::text AS tablename,
    medical_action.medical_case_id,
    medical_action.bovine_id,
    'Action'::text AS type,
    medical_action.event_time,
    medical_action.update_time,
    (medical_action_type.action)::text AS text,
    medical_action.scheduled_userid AS userid
   FROM (bovinemanagement.medical_action
     LEFT JOIN bovinemanagement.medical_action_type ON ((medical_action.medical_action_type_id = medical_action_type.id)))
  WHERE (medical_action.event_time IS NOT NULL)
UNION
 SELECT medical_magnet.id,
    'medical_magnet'::text AS tablename,
    medical_magnet.medical_case_id,
    medical_magnet.bovine_id,
    'Magnet'::text AS type,
    medical_magnet.event_time,
    medical_magnet.update_time,
    ' 1 magnet'::text AS text,
    medical_magnet.userid
   FROM bovinemanagement.medical_magnet
UNION
 SELECT eartag.id,
    'eartag'::text AS tablename,
    NULL::integer AS medical_case_id,
    eartag.bovine_id,
    'Eartag'::text AS type,
    eartag.event_time,
    eartag.update_time,
    ((eartag.side)::text || ' eartag marked to be ordered'::text) AS text,
    eartag.userid
   FROM bovinemanagement.eartag
UNION
 SELECT cull_event.id,
    'cull_event'::text AS tablename,
    NULL::integer AS medical_case_id,
    cull_event.bovine_id,
    'Cull'::text AS type,
    cull_event.event_time,
    cull_event.update_time,
    (((cull_event.reason)::text || '  '::text) || cull_event.comment) AS text,
    cull_event.userid
   FROM bovinemanagement.cull_event
UNION
 SELECT medical_diagnosis.id,
    'medical_diagnosis'::text AS tablename,
    medical_diagnosis.medical_case_id,
    medical_diagnosis.bovine_id,
    'Diagnosis'::text AS type,
    medical_diagnosis.event_time,
    medical_diagnosis.update_time,
    medical_diagnosis_type.diagnosis AS text,
    medical_diagnosis.userid
   FROM (bovinemanagement.medical_diagnosis
     LEFT JOIN bovinemanagement.medical_diagnosis_type ON ((medical_diagnosis.diagnosis_type_id = medical_diagnosis_type.id)))
UNION
 SELECT vet_to_check_event.id,
    'vet_to_check_event'::text AS tablename,
    vet_to_check_event.id AS medical_case_id,
    vet_to_check_event.bovine_id,
    'Vet Check'::text AS type,
    vet_to_check_event.event_time,
    vet_to_check_event.update_time,
    concat_ws(' '::text, 'Why:', vet_to_check_event.comment_entered, 'Outcome:', vet_to_check_event.comment_completed) AS text,
    vet_to_check_event.userid_entered AS userid
   FROM bovinemanagement.vet_to_check_event
  ORDER BY 2, 4, 5;


--
-- Name: medical_summary_latest_record_modified; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.medical_summary_latest_record_modified AS
 SELECT x.id,
    x.tablename,
    x.medical_case_id,
    x.bovine_id,
    x.type,
    x.event_time,
    x.update_time,
    x.text,
    x.userid,
    x.max
   FROM ( SELECT medical_summary.id,
            medical_summary.tablename,
            medical_summary.medical_case_id,
            medical_summary.bovine_id,
            medical_summary.type,
            medical_summary.event_time,
            medical_summary.update_time,
            medical_summary.text,
            medical_summary.userid,
            max(medical_summary.update_time) OVER (PARTITION BY medical_summary.userid) AS max
           FROM bovinemanagement.medical_summary
          WHERE (medical_summary.type <> 'Scheduled Meds'::text)) x
  WHERE (x.max = x.update_time);


--
-- Name: VIEW medical_summary_latest_record_modified; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.medical_summary_latest_record_modified IS 'shows the latest modified records.';


--
-- Name: medicine_administered_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.medicine_administered_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: medicine_administered_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.medicine_administered_id_seq OWNED BY bovinemanagement.medicine_administered.id;


--
-- Name: medicine_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.medicine_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: medicine_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.medicine_id_seq OWNED BY bovinemanagement.medicine.id;


--
-- Name: palpate_comment_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.palpate_comment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: palpate_comment_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.palpate_comment_id_seq OWNED BY bovinemanagement.palpate_comment.id;


--
-- Name: preg_check_comment; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.preg_check_comment (
    id integer NOT NULL,
    comment character varying(256) NOT NULL
);


--
-- Name: TABLE preg_check_comment; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.preg_check_comment IS 'keeps track of different comments for pregnancy checks.';


--
-- Name: preg_check_comment_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.preg_check_comment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: preg_check_comment_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.preg_check_comment_id_seq OWNED BY bovinemanagement.preg_check_comment.id;


--
-- Name: preg_check_event; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.preg_check_event (
    id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    preg_check_type_id integer NOT NULL,
    preg_check_comment_id integer,
    preg_check_comment_custom character varying(256),
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    estimated_breeding_embryo_event_id character(32),
    preg_check_result character varying(8) NOT NULL,
    bovine_id numeric NOT NULL,
    vet_userid character(32),
    CONSTRAINT preg_check_result_check CHECK (((preg_check_result)::bpchar = ANY (ARRAY['pregnant'::bpchar, 'open'::bpchar, 'recheck'::bpchar])))
);


--
-- Name: TABLE preg_check_event; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.preg_check_event IS 'Holds pregnancy check information.';


--
-- Name: COLUMN preg_check_event.preg_check_comment_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.preg_check_event.preg_check_comment_id IS 'optional field.';


--
-- Name: COLUMN preg_check_event.preg_check_comment_custom; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.preg_check_event.preg_check_comment_custom IS 'optional field.';


--
-- Name: COLUMN preg_check_event.estimated_breeding_embryo_event_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.preg_check_event.estimated_breeding_embryo_event_id IS 'what breeding or embryo implant we think caused the pregnancy.';


--
-- Name: COLUMN preg_check_event.preg_check_result; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.preg_check_event.preg_check_result IS 'pregnant,open,recheck';


--
-- Name: COLUMN preg_check_event.vet_userid; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.preg_check_event.vet_userid IS 'if vet performed check';


--
-- Name: preg_check_event_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.preg_check_event_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: preg_check_event_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.preg_check_event_id_seq OWNED BY bovinemanagement.preg_check_event.id;


--
-- Name: preg_check_type; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.preg_check_type (
    id integer NOT NULL,
    test_full_name character varying(128) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE preg_check_type; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.preg_check_type IS 'Lists different types of pregnancy tests.';


--
-- Name: preg_check_type_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.preg_check_type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: preg_check_type_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.preg_check_type_id_seq OWNED BY bovinemanagement.preg_check_type.id;


--
-- Name: pregnant_view; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.pregnant_view AS
 SELECT bovinecurr.id,
    bovinecurr.local_number,
    bovinecurr.full_name,
    preg_check_event.event_time,
    preg_check_type.test_full_name AS preg_check_method,
    combined_breeding_embryo_view.calculated_potential_due_date,
    combined_breeding_embryo_view.service_sire_short_name,
    combined_breeding_embryo_view.service_sire_full_reg_number,
    locationcurr.name AS location_current_name,
    locationcurr.location_id,
    ((preg_check_comment.comment)::text || (preg_check_event.preg_check_comment_custom)::text) AS preg_check_comment
   FROM (((((bovinemanagement.bovinecurr
     LEFT JOIN bovinemanagement.preg_check_event ON ((preg_check_event.id = ( SELECT preg_check_event_1.id
           FROM bovinemanagement.preg_check_event preg_check_event_1
          WHERE ((preg_check_event_1.bovine_id = (bovinecurr.id)::numeric) AND (preg_check_event_1.event_time = ( SELECT max(preg_check_event_2.event_time) AS max
                   FROM bovinemanagement.preg_check_event preg_check_event_2
                  WHERE (preg_check_event_2.bovine_id = (bovinecurr.id)::numeric))))))))
     LEFT JOIN bovinemanagement.preg_check_type ON ((preg_check_type.id = preg_check_event.preg_check_type_id)))
     LEFT JOIN bovinemanagement.preg_check_comment ON ((preg_check_comment.id = preg_check_event.preg_check_comment_id)))
     LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON ((combined_breeding_embryo_view.id = (preg_check_event.estimated_breeding_embryo_event_id)::text)))
     LEFT JOIN bovinemanagement.locationcurr ON (((bovinecurr.id)::numeric = (locationcurr.bovine_id)::numeric)))
  WHERE (((preg_check_event.preg_check_result)::text = 'pregnant'::text) AND (combined_breeding_embryo_view.calculated_potential_due_date >= (bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id) + '200 days'::interval)))
  ORDER BY combined_breeding_embryo_view.calculated_potential_due_date, bovinecurr.local_number;


--
-- Name: production_item; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.production_item (
    event_time timestamp without time zone NOT NULL,
    bovine_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    three_quarter boolean DEFAULT false NOT NULL,
    id integer NOT NULL
);


--
-- Name: production_item_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.production_item_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: production_item_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.production_item_id_seq OWNED BY bovinemanagement.production_item.id;


--
-- Name: sale_price; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.sale_price (
    id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    price numeric,
    plus boolean DEFAULT false NOT NULL,
    comment_id integer,
    custom_comment text,
    userid character(32) NOT NULL,
    bovine_id integer NOT NULL
);


--
-- Name: TABLE sale_price; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.sale_price IS 'sale price of bovines.';


--
-- Name: COLUMN sale_price.price; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.sale_price.price IS 'null means not for sale.';


--
-- Name: COLUMN sale_price.plus; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.sale_price.plus IS 'TRUE when price is a greater amount.';


--
-- Name: COLUMN sale_price.comment_id; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.sale_price.comment_id IS 'standard comments';


--
-- Name: COLUMN sale_price.custom_comment; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.sale_price.custom_comment IS 'user cusomt comment';


--
-- Name: sale_price_comment; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.sale_price_comment (
    id integer NOT NULL,
    comment text NOT NULL
);


--
-- Name: sale_price_comment_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.sale_price_comment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: sale_price_comment_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.sale_price_comment_id_seq OWNED BY bovinemanagement.sale_price_comment.id;


--
-- Name: sale_price_curr; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.sale_price_curr AS
 WITH temp AS (
         SELECT DISTINCT ON (sale_price.bovine_id) sale_price.bovine_id,
            sale_price.event_time,
            sale_price.price,
            sale_price.plus,
            sale_price.custom_comment,
            sale_price.comment_id,
            sale_price.userid,
            sale_price_comment.comment
           FROM (bovinemanagement.sale_price
             LEFT JOIN bovinemanagement.sale_price_comment ON ((sale_price.comment_id = sale_price_comment.id)))
          ORDER BY sale_price.bovine_id, sale_price.event_time DESC, sale_price.price, sale_price.plus, sale_price.custom_comment, sale_price.comment_id, sale_price.userid
        )
 SELECT bovine.id AS bovine_id,
    bovine.local_number,
    bovine.full_name,
    temp.event_time,
    temp.price,
    temp.plus,
    array_to_string(ARRAY[temp.custom_comment, ' '::text, temp.comment], ''::text) AS comment,
    temp.userid
   FROM (temp
     LEFT JOIN bovinemanagement.bovine ON ((temp.bovine_id = bovine.id)));


--
-- Name: VIEW sale_price_curr; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.sale_price_curr IS 'current sale prices';


--
-- Name: sale_price_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.sale_price_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: sale_price_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.sale_price_id_seq OWNED BY bovinemanagement.sale_price.id;


--
-- Name: semen_straw_curr_summary; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.semen_straw_curr_summary AS
 WITH temp AS (
         SELECT DISTINCT semen_straw.id,
            sire.short_name,
            sire.full_name,
            sire.full_reg_number,
            semen_straw.semen_code,
            semen_straw.tank,
            semen_straw.bin,
            semen_straw.freeze_date,
            aggregate_view_all.lpi,
            aggregate_view_all.prodoll,
            aggregate_view_all.reliability AS reliability_percent,
            semen_straw.invoice_cost,
            sire_semen_code.sexed_semen,
            semen_straw.reserved,
            semen_straw.ownerid,
            semen_straw.projection
           FROM (((bovinemanagement.semen_straw
             LEFT JOIN bovinemanagement.sire_semen_code ON ((sire_semen_code.semen_code = semen_straw.semen_code)))
             LEFT JOIN bovinemanagement.sire ON ((sire.full_reg_number = sire_semen_code.sire_full_reg_number)))
             LEFT JOIN batch.aggregate_view_all ON (((aggregate_view_all.full_reg_number)::bpchar = sire.full_reg_number)))
          WHERE ((semen_straw.breeding_event_id IS NULL) AND (semen_straw.discarded IS FALSE))
        )
 SELECT temp.short_name,
    temp.full_name,
    temp.full_reg_number,
    temp.semen_code,
    temp.tank,
    temp.bin,
    temp.freeze_date,
    temp.lpi,
    temp.prodoll,
    temp.reliability_percent,
    temp.invoice_cost,
    temp.sexed_semen,
    temp.reserved,
    temp.ownerid,
    temp.projection,
    count(*) AS count
   FROM temp
  GROUP BY temp.short_name, temp.full_name, temp.full_reg_number, temp.semen_code, temp.tank, temp.bin, temp.freeze_date, temp.lpi, temp.prodoll, temp.reliability_percent, temp.invoice_cost, temp.sexed_semen, temp.reserved, temp.ownerid, temp.projection
  ORDER BY temp.short_name;


--
-- Name: VIEW semen_straw_curr_summary; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.semen_straw_curr_summary IS 'current inventory of semen straws summary countability';


--
-- Name: semen_straw_curr_summary_available; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.semen_straw_curr_summary_available AS
 SELECT semen_straw_curr_summary.short_name,
    semen_straw_curr_summary.full_name,
    semen_straw_curr_summary.full_reg_number,
    semen_straw_curr_summary.semen_code,
    semen_straw_curr_summary.tank,
    semen_straw_curr_summary.bin,
    semen_straw_curr_summary.freeze_date,
    semen_straw_curr_summary.lpi,
    semen_straw_curr_summary.prodoll,
    semen_straw_curr_summary.reliability_percent,
    semen_straw_curr_summary.invoice_cost,
    semen_straw_curr_summary.sexed_semen,
    semen_straw_curr_summary.reserved,
    semen_straw_curr_summary.ownerid,
    semen_straw_curr_summary.projection,
    semen_straw_curr_summary.count
   FROM bovinemanagement.semen_straw_curr_summary
  WHERE ((semen_straw_curr_summary.bin IS NOT NULL) AND (semen_straw_curr_summary.tank <> 'Z'::bpchar));


--
-- Name: VIEW semen_straw_curr_summary_available; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.semen_straw_curr_summary_available IS 'not in bottom of tank or projection';


--
-- Name: semen_straw_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.semen_straw_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: semen_straw_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.semen_straw_id_seq OWNED BY bovinemanagement.semen_straw.id;


--
-- Name: service_picks; Type: TABLE; Schema: bovinemanagement; Owner: -
--

CREATE TABLE bovinemanagement.service_picks (
    id integer NOT NULL,
    bovine_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    recipient boolean DEFAULT false NOT NULL,
    primary_one character(18),
    primary_two character(18),
    primary_three character(18),
    secondary_one character(18),
    secondary_two character(18),
    secondary_three character(18),
    userid character varying(32) NOT NULL,
    donotbreed boolean DEFAULT false NOT NULL,
    event_time timestamp without time zone NOT NULL,
    donor boolean DEFAULT false NOT NULL
);


--
-- Name: TABLE service_picks; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON TABLE bovinemanagement.service_picks IS 'holds information on what sires cows should be bread too.';


--
-- Name: COLUMN service_picks.recipient; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.service_picks.recipient IS 'true means cow is marked as recipient.';


--
-- Name: COLUMN service_picks.primary_one; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.service_picks.primary_one IS 'reg number of sire to breed too.';


--
-- Name: COLUMN service_picks.donotbreed; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON COLUMN bovinemanagement.service_picks.donotbreed IS 'whether to breed or not';


--
-- Name: service_picks_curr; Type: VIEW; Schema: bovinemanagement; Owner: -
--

CREATE VIEW bovinemanagement.service_picks_curr AS
 SELECT DISTINCT ON (a.bovine_id) a.id,
    a.bovine_id,
    a.create_time,
    a.update_time,
    a.event_time,
    a.recipient,
    a.donotbreed,
    a.donor,
    a.primary_one,
    a.primary_two,
    a.primary_three,
    a.secondary_one,
    a.secondary_two,
    a.secondary_three,
    a.userid
   FROM (bovinemanagement.service_picks a
     JOIN bovinemanagement.service_picks ON ((service_picks.id = a.id)))
  ORDER BY a.bovine_id, a.event_time DESC;


--
-- Name: VIEW service_picks_curr; Type: COMMENT; Schema: bovinemanagement; Owner: -
--

COMMENT ON VIEW bovinemanagement.service_picks_curr IS 'All the current service_picks';


--
-- Name: service_picks_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.service_picks_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: service_picks_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.service_picks_id_seq OWNED BY bovinemanagement.service_picks.id;


--
-- Name: sire_id_seq; Type: SEQUENCE; Schema: bovinemanagement; Owner: -
--

CREATE SEQUENCE bovinemanagement.sire_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: sire_id_seq; Type: SEQUENCE OWNED BY; Schema: bovinemanagement; Owner: -
--

ALTER SEQUENCE bovinemanagement.sire_id_seq OWNED BY bovinemanagement.sire.id;


--
-- Name: datum; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.datum (
    id integer NOT NULL,
    geom gis.geometry,
    CONSTRAINT enforce_dims_geo CHECK ((gis.st_ndims(geom) = 3)),
    CONSTRAINT enforce_geotype_geo CHECK (((gis.geometrytype(geom) = 'POLYGON'::text) OR (geom IS NULL))),
    CONSTRAINT enforce_srid_geo CHECK ((gis.st_srid(geom) = 4326))
);


--
-- Name: TABLE datum; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.datum IS 'stores polygons of field boundaries.';


--
-- Name: cropping_id_seq; Type: SEQUENCE; Schema: cropping; Owner: -
--

CREATE SEQUENCE cropping.cropping_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: cropping_id_seq; Type: SEQUENCE OWNED BY; Schema: cropping; Owner: -
--

ALTER SEQUENCE cropping.cropping_id_seq OWNED BY cropping.datum.id;


--
-- Name: border_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.border_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE border_event; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.border_event IS 'holds field borders';


--
-- Name: comment_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.comment_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text
);


--
-- Name: fertilizer_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.fertilizer_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    fertilizer_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text,
    fertilizer_application_method_id integer NOT NULL,
    amount numeric NOT NULL,
    fertilizer_userid character(32) NOT NULL,
    all_covered boolean DEFAULT true NOT NULL
);


--
-- Name: COLUMN fertilizer_event.amount; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer_event.amount IS 'in kg/ha';


--
-- Name: COLUMN fertilizer_event.all_covered; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer_event.all_covered IS 'was field all done?';


--
-- Name: fertilizer_event_scheduled; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.fertilizer_event_scheduled (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    fertilizer_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text,
    fertilizer_application_method_id integer NOT NULL,
    amount numeric NOT NULL
);


--
-- Name: general_comment_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.general_comment_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text NOT NULL
);


--
-- Name: harvest_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.harvest_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text
);


--
-- Name: lime_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.lime_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    lime_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text,
    spreader_userid character(32) NOT NULL,
    lime_amount numeric NOT NULL
);


--
-- Name: manure_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.manure_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    manure_amount numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    spreader_userid character(32) NOT NULL,
    comment text,
    manure_type text,
    all_covered boolean DEFAULT true NOT NULL
);


--
-- Name: COLUMN manure_event.manure_amount; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.manure_event.manure_amount IS 'liters per hecate';


--
-- Name: COLUMN manure_event.spreader_userid; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.manure_event.spreader_userid IS 'who did the spreading';


--
-- Name: COLUMN manure_event.manure_type; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.manure_event.manure_type IS 'liquid or semi-solid';


--
-- Name: COLUMN manure_event.all_covered; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.manure_event.all_covered IS 'to do this properly you would use GIS, but this is more practical for users.';


--
-- Name: seed_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.seed_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    seed_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text,
    population numeric,
    amount numeric,
    seeder_userid character(32) NOT NULL
);


--
-- Name: TABLE seed_event; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.seed_event IS 'tracks when seeding occurs.';


--
-- Name: COLUMN seed_event.event_time; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.seed_event.event_time IS 'when seeding occured';


--
-- Name: COLUMN seed_event.population; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.seed_event.population IS 'population per hectare (where applicable)';


--
-- Name: COLUMN seed_event.amount; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.seed_event.amount IS 'kg per hectare (where applicable)';


--
-- Name: COLUMN seed_event.seeder_userid; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.seed_event.seeder_userid IS 'who actually did the seeding.';


--
-- Name: seed_event_scheduled; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.seed_event_scheduled (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    seed_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text
);


--
-- Name: soil_sample_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.soil_sample_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer,
    datum_id integer,
    userid character varying(32),
    event_time timestamp without time zone,
    report_date date,
    ph numeric,
    cec numeric,
    phosphorus numeric,
    potassium numeric,
    calcium numeric,
    magnesium numeric,
    boron numeric,
    copper numeric,
    zinc numeric,
    sulphur numeric,
    manganese numeric,
    iron numeric,
    aluminum numeric,
    sodium numeric,
    buffer_ph numeric,
    create_time timestamp without time zone,
    update_time timestamp without time zone,
    organic_matter numeric
);


--
-- Name: COLUMN soil_sample_event.event_time; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.soil_sample_event.event_time IS 'sample date';


--
-- Name: COLUMN soil_sample_event.buffer_ph; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.soil_sample_event.buffer_ph IS 'aka soil index on soil test reports';


--
-- Name: COLUMN soil_sample_event.organic_matter; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.soil_sample_event.organic_matter IS 'in percent (optional)';


--
-- Name: spray_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.spray_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    amount numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    spray_id integer NOT NULL,
    comment text,
    sprayer_userid character(32) NOT NULL
);


--
-- Name: COLUMN spray_event.amount; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.spray_event.amount IS 'amount of actual chemical per liter.';


--
-- Name: COLUMN spray_event.sprayer_userid; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.spray_event.sprayer_userid IS 'who actually did the spraying';


--
-- Name: tillage_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.tillage_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    datum_id integer NOT NULL,
    tillage_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text,
    tillage_userid character(32) NOT NULL
);


--
-- Name: COLUMN tillage_event.tillage_userid; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.tillage_event.tillage_userid IS 'who actually did the tillage';


--
-- Name: all_event; Type: VIEW; Schema: cropping; Owner: -
--

CREATE VIEW cropping.all_event AS
 SELECT border_event.id,
    'border_event'::text AS table_id,
    'Border Change'::text AS event_name,
    border_event.field_id,
    border_event.datum_id,
    border_event.event_time
   FROM cropping.border_event
UNION
 SELECT seed_event.id,
    'seed_event'::text AS table_id,
    'Seed'::text AS event_name,
    seed_event.field_id,
    seed_event.datum_id,
    seed_event.event_time
   FROM cropping.seed_event
UNION
 SELECT manure_event.id,
    'manure_event'::text AS table_id,
    'Manure'::text AS event_name,
    manure_event.field_id,
    manure_event.datum_id,
    manure_event.event_time
   FROM cropping.manure_event
UNION
 SELECT tillage_event.id,
    'tillage_event'::text AS table_id,
    'Tillage'::text AS event_name,
    tillage_event.field_id,
    tillage_event.datum_id,
    tillage_event.event_time
   FROM cropping.tillage_event
UNION
 SELECT seed_event_scheduled.id,
    'seed_event_scheduled'::text AS table_id,
    'Seeding Scheduled'::text AS event_name,
    seed_event_scheduled.field_id,
    seed_event_scheduled.datum_id,
    seed_event_scheduled.event_time
   FROM cropping.seed_event_scheduled
UNION
 SELECT spray_event.id,
    'spray_event'::text AS table_id,
    'Spraying'::text AS event_name,
    spray_event.field_id,
    spray_event.datum_id,
    spray_event.event_time
   FROM cropping.spray_event
UNION
 SELECT harvest_event.id,
    'harvest_event'::text AS table_id,
    'Harvest'::text AS event_name,
    harvest_event.field_id,
    harvest_event.datum_id,
    harvest_event.event_time
   FROM cropping.harvest_event
UNION
 SELECT fertilizer_event.id,
    'fertilizer_event'::text AS table_id,
    'Fertilizer'::text AS event_name,
    fertilizer_event.field_id,
    fertilizer_event.datum_id,
    fertilizer_event.event_time
   FROM cropping.fertilizer_event
UNION
 SELECT fertilizer_event_scheduled.id,
    'fertilizer_event_scheduled'::text AS table_id,
    'Fertilizer Scheduled'::text AS event_name,
    fertilizer_event_scheduled.field_id,
    fertilizer_event_scheduled.datum_id,
    fertilizer_event_scheduled.event_time
   FROM cropping.fertilizer_event_scheduled
UNION
 SELECT comment_event.id,
    'comment_event'::text AS table_id,
    'Comment'::text AS event_name,
    comment_event.field_id,
    comment_event.datum_id,
    comment_event.event_time
   FROM cropping.comment_event
UNION
 SELECT lime_event.id,
    'lime_event'::text AS table_id,
    'Lime'::text AS event_name,
    lime_event.field_id,
    lime_event.datum_id,
    lime_event.event_time
   FROM cropping.lime_event
UNION
 SELECT soil_sample_event.id,
    'soil_sample_event'::text AS table_id,
    'Soil Sample'::text AS event_name,
    soil_sample_event.field_id,
    soil_sample_event.datum_id,
    soil_sample_event.event_time
   FROM cropping.soil_sample_event
UNION
 SELECT general_comment_event.id,
    'general_comment_event'::text AS table_id,
    'General Comment'::text AS event_name,
    general_comment_event.field_id,
    general_comment_event.datum_id,
    general_comment_event.event_time
   FROM cropping.general_comment_event;


--
-- Name: fertilizer; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.fertilizer (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    n integer NOT NULL,
    p integer NOT NULL,
    k integer NOT NULL,
    n_source text DEFAULT 'ammonium nitrate'::text NOT NULL,
    b numeric,
    s numeric,
    zn numeric
);


--
-- Name: TABLE fertilizer; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.fertilizer IS 'stores types of fertilizer';


--
-- Name: COLUMN fertilizer.n; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer.n IS 'nitrogen';


--
-- Name: COLUMN fertilizer.p; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer.p IS 'phosphorus';


--
-- Name: COLUMN fertilizer.k; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer.k IS 'potassium';


--
-- Name: COLUMN fertilizer.n_source; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer.n_source IS 'ammonium nitrate or urea';


--
-- Name: COLUMN fertilizer.b; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer.b IS 'boron, example 0.6 ';


--
-- Name: COLUMN fertilizer.s; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer.s IS 'sulphur';


--
-- Name: COLUMN fertilizer.zn; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer.zn IS 'Zinc';


--
-- Name: fertilizer_application_method; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.fertilizer_application_method (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    name text
);


--
-- Name: fertilizer_display; Type: VIEW; Schema: cropping; Owner: -
--

CREATE VIEW cropping.fertilizer_display AS
 WITH temp AS (
         SELECT fertilizer.id,
            fertilizer.n,
            fertilizer.p,
            fertilizer.k,
            fertilizer.n_source,
            fertilizer.b,
            fertilizer.s,
            fertilizer.zn,
                CASE
                    WHEN (fertilizer.n_source = 'ammonium nitrate'::text) THEN 'AN'::text
                    WHEN (fertilizer.n_source = 'ammonium sulfate'::text) THEN 'AS'::text
                    WHEN (fertilizer.n_source = 'urea'::text) THEN 'U'::text
                    WHEN (fertilizer.n_source IS NULL) THEN ''::text
                    ELSE 'Unknow N source'::text
                END AS n_source_short,
                CASE
                    WHEN (fertilizer.b IS NULL) THEN ''::text
                    ELSE (fertilizer.b || 'b'::text)
                END AS b_display,
                CASE
                    WHEN (fertilizer.s IS NULL) THEN ''::text
                    ELSE (fertilizer.s || 's'::text)
                END AS s_display,
                CASE
                    WHEN (fertilizer.zn IS NULL) THEN ''::text
                    ELSE (fertilizer.zn || 'zn'::text)
                END AS zn_display
           FROM cropping.fertilizer
        )
 SELECT temp.id,
    ((((((((((((temp.n || '-'::text) || temp.p) || '-'::text) || temp.k) || ' '::text) || temp.n_source_short) || ' '::text) || temp.b_display) || ' '::text) || temp.s_display) || ' '::text) || temp.zn_display) AS fertilizer_display
   FROM temp;


--
-- Name: VIEW fertilizer_display; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON VIEW cropping.fertilizer_display IS 'display  fertilizer types for displaying to users. DOES NOT SUPPORT different types of sulphur sources.';


--
-- Name: fertilizer_recommendation; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.fertilizer_recommendation (
    id integer NOT NULL,
    element character varying(64) NOT NULL,
    element_range numrange NOT NULL,
    recommendation numeric NOT NULL
);


--
-- Name: TABLE fertilizer_recommendation; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.fertilizer_recommendation IS 'table of values to recommend fertilizer for sol tests.';


--
-- Name: COLUMN fertilizer_recommendation.id; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer_recommendation.id IS 'id that points to different crops';


--
-- Name: COLUMN fertilizer_recommendation.element_range; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer_recommendation.element_range IS 'soil test ppm element range';


--
-- Name: COLUMN fertilizer_recommendation.recommendation; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.fertilizer_recommendation.recommendation IS 'in kg/ha of element to put on field';


--
-- Name: fertilizer_recommendation_crop; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.fertilizer_recommendation_crop (
    specific_type character varying(255) NOT NULL,
    fertilizer_recommendation_id integer NOT NULL,
    comment character varying
);


--
-- Name: TABLE fertilizer_recommendation_crop; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.fertilizer_recommendation_crop IS 'crops pointing to recomendations';


--
-- Name: fertilizer_removal_rate; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.fertilizer_removal_rate (
    crop text NOT NULL,
    "N" numeric,
    "P" numeric NOT NULL,
    "K" numeric NOT NULL
);


--
-- Name: TABLE fertilizer_removal_rate; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.fertilizer_removal_rate IS 'iowa state crop removal rates in kg/ha per element.';


--
-- Name: field; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.field (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    alpha_numeric_id character varying NOT NULL,
    common_name character varying NOT NULL,
    major_name character varying NOT NULL,
    old_field_id integer,
    active boolean DEFAULT true NOT NULL
);


--
-- Name: TABLE field; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.field IS 'fields';


--
-- Name: field_parameter; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.field_parameter (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    pasture boolean DEFAULT false NOT NULL,
    not_normally_farmed boolean DEFAULT false NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    alfalfa_suitable boolean DEFAULT false NOT NULL,
    corn_suitable boolean DEFAULT false NOT NULL,
    trefoil_suitable boolean DEFAULT false NOT NULL,
    drainage_score numeric DEFAULT 3 NOT NULL,
    spfh_suitable boolean DEFAULT false NOT NULL
);


--
-- Name: TABLE field_parameter; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.field_parameter IS 'stores extra info about fields that can change with time.';


--
-- Name: COLUMN field_parameter.pasture; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.field_parameter.pasture IS 'mark as for pasture';


--
-- Name: COLUMN field_parameter.not_normally_farmed; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.field_parameter.not_normally_farmed IS 'for pieces that we do not normally mow in a year.';


--
-- Name: COLUMN field_parameter.alfalfa_suitable; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.field_parameter.alfalfa_suitable IS 'can it grow alfalfa?';


--
-- Name: COLUMN field_parameter.corn_suitable; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.field_parameter.corn_suitable IS 'can it grow corn?';


--
-- Name: COLUMN field_parameter.trefoil_suitable; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.field_parameter.trefoil_suitable IS 'can it grow trefoil?';


--
-- Name: COLUMN field_parameter.drainage_score; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.field_parameter.drainage_score IS 'score 1=interval or tile . Score 5=Wet all the time.';


--
-- Name: COLUMN field_parameter.spfh_suitable; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.field_parameter.spfh_suitable IS 'Do we use this field for silage with SPFH?';


--
-- Name: field_parameter_curr; Type: VIEW; Schema: cropping; Owner: -
--

CREATE VIEW cropping.field_parameter_curr AS
 SELECT DISTINCT ON (field_parameter.field_id) field_parameter.id,
    field_parameter.field_id,
    field_parameter.event_time,
    field_parameter.pasture,
    field_parameter.not_normally_farmed,
    field_parameter.create_time,
    field_parameter.update_time,
    field_parameter.userid,
    field_parameter.alfalfa_suitable,
    field_parameter.corn_suitable,
    field_parameter.trefoil_suitable,
    field_parameter.drainage_score,
    field_parameter.spfh_suitable
   FROM cropping.field_parameter
  ORDER BY field_parameter.field_id, field_parameter.event_time DESC;


--
-- Name: VIEW field_parameter_curr; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON VIEW cropping.field_parameter_curr IS 'table with latest field parameter event for each field. NOTE, this will show field param events way in future if that is the latest event.';


--
-- Name: fieldcurr; Type: VIEW; Schema: cropping; Owner: -
--

CREATE VIEW cropping.fieldcurr AS
 WITH temp AS (
         SELECT field.id,
            field.alpha_numeric_id,
            field.common_name,
            field.major_name,
            field.old_field_id,
            field.active,
            ( SELECT border_1.id
                   FROM cropping.border_event border_1
                  WHERE ((border_1.field_id = field.id) AND (border_1.event_time = ( SELECT max(border_2.event_time) AS max
                           FROM cropping.border_event border_2
                          WHERE ((border_2.field_id = field.id) AND (border_2.event_time <= ('now'::text)::date)))))) AS border_id,
            ( SELECT seed_event_1.id
                   FROM cropping.seed_event seed_event_1
                  WHERE ((seed_event_1.field_id = field.id) AND (seed_event_1.event_time = ( SELECT max(seed_event_2.event_time) AS max
                           FROM cropping.seed_event seed_event_2
                          WHERE ((seed_event_2.field_id = field.id) AND (seed_event_2.event_time <= (('now'::text)::date + '1 day'::interval))))))) AS seed_event_id,
            ( SELECT field_parameter_1.id
                   FROM cropping.field_parameter field_parameter_1
                  WHERE ((field_parameter_1.field_id = field.id) AND (field_parameter_1.event_time = ( SELECT max(field_parameter_2.event_time) AS max
                           FROM cropping.field_parameter field_parameter_2
                          WHERE ((field_parameter_2.field_id = field.id) AND (field_parameter_2.event_time <= ('now'::text)::date)))))) AS field_parameter_id
           FROM cropping.field
          WHERE (field.active = true)
        )
 SELECT temp.id,
    temp.alpha_numeric_id,
    temp.common_name,
    temp.major_name,
    temp.old_field_id,
    temp.active,
    datum.id AS border_datum_id,
    temp.border_id,
    temp.seed_event_id,
    border.event_time AS border_event_time,
    datum.geom AS border_geom,
    seed_event.event_time AS seed_event_event_time,
    datum.geom AS seed_event_geom,
    field_parameter.pasture,
    field_parameter.not_normally_farmed
   FROM (((((temp
     LEFT JOIN cropping.border_event border ON ((border.id = temp.border_id)))
     LEFT JOIN cropping.datum ON ((border.datum_id = datum.id)))
     LEFT JOIN cropping.seed_event ON ((seed_event.id = temp.seed_event_id)))
     LEFT JOIN cropping.datum datum2 ON ((seed_event.datum_id = datum2.id)))
     LEFT JOIN cropping.field_parameter ON ((field_parameter.id = temp.field_parameter_id)))
  ORDER BY ((substr((temp.alpha_numeric_id)::text, 0, (strpos((temp.alpha_numeric_id)::text, '-'::text) + 1)))::character(5)), (substr((temp.alpha_numeric_id)::text, strpos((temp.alpha_numeric_id)::text, '-'::text)))::integer DESC;


--
-- Name: VIEW fieldcurr; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON VIEW cropping.fieldcurr IS 'Shows fields in the current state based on today''s date. Future fields will not be shown.';


--
-- Name: foragecurr; Type: VIEW; Schema: cropping; Owner: -
--

CREATE VIEW cropping.foragecurr AS
 SELECT fieldcurr.id,
    fieldcurr.alpha_numeric_id,
    fieldcurr.common_name,
    fieldcurr.major_name,
    fieldcurr.old_field_id,
    fieldcurr.active,
    fieldcurr.border_datum_id,
    fieldcurr.border_id,
    fieldcurr.seed_event_id,
    fieldcurr.border_event_time,
    fieldcurr.border_geom,
    fieldcurr.seed_event_event_time,
    fieldcurr.seed_event_geom,
    fieldcurr.pasture,
    fieldcurr.not_normally_farmed,
    round(((gis.st_area(gis.st_transform(fieldcurr.border_geom, 2036)) / (10000)::double precision))::numeric, 2) AS area,
    field_parameter.spfh_suitable,
    field_parameter.alfalfa_suitable,
    field_parameter.corn_suitable,
    field_parameter.trefoil_suitable
   FROM (cropping.fieldcurr
     LEFT JOIN cropping.field_parameter ON ((field_parameter.id = cropping.field_parameter_id_at_timestamp(fieldcurr.id, (now())::timestamp without time zone))))
  WHERE ((field_parameter.alfalfa_suitable = true) OR (field_parameter.corn_suitable = true) OR (field_parameter.trefoil_suitable = true));


--
-- Name: VIEW foragecurr; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON VIEW cropping.foragecurr IS 'fields we can run a baler,forage harvester, or combine to harvest a crop.
';


--
-- Name: lime; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.lime (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    name text NOT NULL
);


--
-- Name: pasturecurr; Type: VIEW; Schema: cropping; Owner: -
--

CREATE VIEW cropping.pasturecurr AS
 SELECT fieldcurr.id,
    fieldcurr.alpha_numeric_id,
    fieldcurr.common_name,
    fieldcurr.major_name,
    fieldcurr.old_field_id,
    fieldcurr.active,
    fieldcurr.border_datum_id,
    fieldcurr.border_id,
    fieldcurr.seed_event_id,
    fieldcurr.border_event_time,
    fieldcurr.border_geom,
    fieldcurr.seed_event_event_time,
    fieldcurr.seed_event_geom,
    fieldcurr.pasture,
    fieldcurr.not_normally_farmed,
    round(((gis.st_area(gis.st_transform(fieldcurr.border_geom, 2036)) / (10000)::double precision))::numeric, 2) AS area,
    field_parameter.spfh_suitable,
    field_parameter.alfalfa_suitable,
    field_parameter.corn_suitable,
    field_parameter.trefoil_suitable
   FROM (cropping.fieldcurr
     LEFT JOIN cropping.field_parameter ON ((field_parameter.id = cropping.field_parameter_id_at_timestamp(fieldcurr.id, (now())::timestamp without time zone))))
  WHERE (field_parameter.pasture = true);


--
-- Name: VIEW pasturecurr; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON VIEW cropping.pasturecurr IS 'anything that is currently a pasture';


--
-- Name: plant_tissue_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.plant_tissue_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer,
    datum_id integer,
    userid character varying(32),
    event_time timestamp without time zone,
    report_date date,
    nitrogen numeric NOT NULL,
    phosphorus numeric NOT NULL,
    potassium numeric NOT NULL,
    calcium numeric,
    magnesium numeric,
    boron numeric,
    copper numeric,
    zinc numeric,
    iron numeric,
    manganese numeric,
    create_time timestamp without time zone,
    update_time timestamp without time zone,
    sodium numeric,
    comment text,
    plant_tissue_parameter_id integer NOT NULL,
    sulphur numeric
);


--
-- Name: COLUMN plant_tissue_event.nitrogen; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.plant_tissue_event.nitrogen IS 'in percent';


--
-- Name: COLUMN plant_tissue_event.phosphorus; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.plant_tissue_event.phosphorus IS 'in percent';


--
-- Name: COLUMN plant_tissue_event.potassium; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.plant_tissue_event.potassium IS 'in percent';


--
-- Name: COLUMN plant_tissue_event.calcium; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.plant_tissue_event.calcium IS 'in percent';


--
-- Name: COLUMN plant_tissue_event.magnesium; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.plant_tissue_event.magnesium IS 'in percent';


--
-- Name: COLUMN plant_tissue_event.sodium; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.plant_tissue_event.sodium IS 'in percent';


--
-- Name: COLUMN plant_tissue_event.comment; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.plant_tissue_event.comment IS 'optional comment';


--
-- Name: COLUMN plant_tissue_event.sulphur; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.plant_tissue_event.sulphur IS 'percent';


--
-- Name: plant_tissue_parameter; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.plant_tissue_parameter (
    id numeric DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    seed_category_id integer NOT NULL,
    crop_growth_phase text NOT NULL,
    sample_note text,
    nitrogen_min numeric NOT NULL,
    nitrogen_max numeric NOT NULL,
    nitrogen_median numeric NOT NULL,
    nitrogen_sufficient_min numeric NOT NULL,
    nitrogen_sufficient_max numeric NOT NULL,
    phosphorus_min numeric NOT NULL,
    phosphorus_max numeric NOT NULL,
    phosphorus_median numeric NOT NULL,
    phosphorus_sufficient_min numeric NOT NULL,
    phosphorus_sufficient_max numeric NOT NULL,
    potassium_min numeric NOT NULL,
    potassium_max numeric NOT NULL,
    potassium_median numeric NOT NULL,
    potassium_sufficient_min numeric NOT NULL,
    potassium_sufficient_max numeric NOT NULL,
    calcium_min numeric NOT NULL,
    calcium_max numeric NOT NULL,
    calcium_median numeric NOT NULL,
    calcium_sufficient_min numeric NOT NULL,
    calcium_sufficient_max numeric NOT NULL,
    magnesium_min numeric NOT NULL,
    magnesium_max numeric NOT NULL,
    magnesium_median numeric NOT NULL,
    magnesium_sufficient_min numeric NOT NULL,
    magnesium_sufficient_max numeric NOT NULL,
    boron_min numeric NOT NULL,
    boron_max numeric NOT NULL,
    boron_median numeric NOT NULL,
    boron_sufficient_min numeric NOT NULL,
    boron_sufficient_max numeric NOT NULL,
    copper_min numeric NOT NULL,
    copper_max numeric NOT NULL,
    copper_median numeric NOT NULL,
    copper_sufficient_min numeric NOT NULL,
    copper_sufficient_max numeric NOT NULL,
    zinc_min numeric NOT NULL,
    zinc_max numeric NOT NULL,
    zinc_median numeric NOT NULL,
    zinc_sufficient_min numeric NOT NULL,
    zinc_sufficient_max numeric NOT NULL,
    iron_min numeric NOT NULL,
    iron_max numeric NOT NULL,
    iron_median numeric NOT NULL,
    iron_sufficient_min numeric NOT NULL,
    iron_sufficient_max numeric NOT NULL,
    manganese_min numeric NOT NULL,
    manganese_max numeric NOT NULL,
    manganese_median numeric NOT NULL,
    manganese_sufficient_min numeric NOT NULL,
    manganese_sufficient_max numeric NOT NULL
);


--
-- Name: TABLE plant_tissue_parameter; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.plant_tissue_parameter IS 'ranges for idea plant tissue sample.';


--
-- Name: COLUMN plant_tissue_parameter.nitrogen_min; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.plant_tissue_parameter.nitrogen_min IS 'percent';


--
-- Name: seed; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.seed (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    seed_category_id integer NOT NULL,
    name text NOT NULL,
    chu numeric,
    company text,
    active boolean DEFAULT true NOT NULL,
    trait_rr boolean DEFAULT false NOT NULL,
    trait_bt boolean DEFAULT false NOT NULL,
    note text
);


--
-- Name: TABLE seed; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.seed IS 'different types of seed.';


--
-- Name: COLUMN seed.chu; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.seed.chu IS 'crop heat units';


--
-- Name: COLUMN seed.company; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.seed.company IS 'name of company who makes seed';


--
-- Name: seed_category; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.seed_category (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    general_type character varying(255) NOT NULL,
    specific_type character varying(255) NOT NULL,
    perennial boolean NOT NULL,
    google_polygon_outline_colour character(8) DEFAULT 'fffefefe'::bpchar NOT NULL,
    google_polygon_fill_colour character(8) NOT NULL,
    rotational_crop boolean DEFAULT false NOT NULL
);


--
-- Name: COLUMN seed_category.rotational_crop; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.seed_category.rotational_crop IS 'true if seed is used as part of a rotation.';


--
-- Name: rotationcurr; Type: VIEW; Schema: cropping; Owner: -
--

CREATE VIEW cropping.rotationcurr AS
 WITH temp AS (
         SELECT fieldcurr.id,
            fieldcurr.alpha_numeric_id,
            cropping.rotation_establishment_event(fieldcurr.id, (date_part('year'::text, now()))::integer) AS rotation_establishment_event
           FROM cropping.fieldcurr
        )
 SELECT temp.id,
    temp.alpha_numeric_id,
    temp.rotation_establishment_event,
    seed_category.general_type,
    seed_category.specific_type,
    seed.seed_category_id,
    seed_event.event_time
   FROM (((temp
     LEFT JOIN cropping.seed_event ON ((seed_event.id = temp.rotation_establishment_event)))
     LEFT JOIN cropping.seed ON ((seed.id = seed_event.seed_id)))
     LEFT JOIN cropping.seed_category ON ((seed_category.id = seed.seed_category_id)))
  WHERE (temp.rotation_establishment_event IS NOT NULL)
  ORDER BY ((substr((temp.alpha_numeric_id)::text, 0, (strpos((temp.alpha_numeric_id)::text, '-'::text) + 1)))::character(5)), (substr((temp.alpha_numeric_id)::text, strpos((temp.alpha_numeric_id)::text, '-'::text)))::integer DESC;


--
-- Name: VIEW rotationcurr; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON VIEW cropping.rotationcurr IS 'lists the current rotation crops starting stand.';


--
-- Name: seed_curr; Type: VIEW; Schema: cropping; Owner: -
--

CREATE VIEW cropping.seed_curr AS
 SELECT seed_event.id,
    seed_event.event_time,
    datum.geom
   FROM (cropping.seed_event
     LEFT JOIN cropping.datum ON ((datum.id = seed_event.datum_id)))
  WHERE (datum.geom IS NOT NULL);


--
-- Name: seed_event_curr; Type: VIEW; Schema: cropping; Owner: -
--

CREATE VIEW cropping.seed_event_curr AS
 SELECT xxx.id,
    xxx.field_id,
    xxx.datum_id,
    xxx.seed_id,
    xxx.create_time,
    xxx.update_time,
    xxx.userid,
    xxx.event_time,
    xxx.comment,
    xxx.population,
    xxx.amount,
    seed.seed_category_id,
    seed.name,
    seed.chu,
    seed.company AS active,
    seed.trait_rr,
    seed.trait_bt,
    seed.note,
    seed_category.general_type,
    seed_category.specific_type,
    seed_category.perennial,
    seed_category.google_polygon_outline_colour,
    seed_category.google_polygon_fill_colour
   FROM ((cropping.seed_event xxx
     LEFT JOIN cropping.seed ON ((seed.id = xxx.seed_id)))
     LEFT JOIN cropping.seed_category ON ((seed_category.id = seed.seed_category_id)))
  WHERE (xxx.id = ( SELECT seed_event.id
           FROM cropping.seed_event
          WHERE ((seed_event.field_id = xxx.field_id) AND (seed_event.event_time = ( SELECT max(seed_event_1.event_time) AS max
                   FROM cropping.seed_event seed_event_1
                  WHERE (seed_event_1.field_id = xxx.field_id))))));


--
-- Name: seed_inventory; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.seed_inventory (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    seed_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    bag_count numeric NOT NULL
);


--
-- Name: TABLE seed_inventory; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.seed_inventory IS 'holds inventory of seeds. mostly for corn.';


--
-- Name: COLUMN seed_inventory.bag_count; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.seed_inventory.bag_count IS 'number of bangs currently on hand';


--
-- Name: spray; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.spray (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    name character varying NOT NULL,
    active_chemical character varying NOT NULL,
    active_chemical2 character varying,
    default_rate_ha numeric NOT NULL,
    default_rate_tank numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL
);


--
-- Name: COLUMN spray.name; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.spray.name IS 'name of spray (pesticide)';


--
-- Name: COLUMN spray.active_chemical; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.spray.active_chemical IS 'ex. glyphosate 540 g/l';


--
-- Name: COLUMN spray.default_rate_ha; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.spray.default_rate_ha IS 'rater of chemical liters per ha (no water).';


--
-- Name: COLUMN spray.default_rate_tank; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.spray.default_rate_tank IS 'rate to dump in sprayer tank in liters';


--
-- Name: tillage; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.tillage (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    name text NOT NULL
);


--
-- Name: yield_event; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.yield_event (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    field_id integer NOT NULL,
    yield_type_id integer NOT NULL,
    create_time timestamp(6) without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    comment character varying,
    userid character(32) NOT NULL,
    amount numeric NOT NULL,
    event_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE yield_event; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON TABLE cropping.yield_event IS 'holds yield info for field, for non-bagged crops, ie combine grain, round bales, etc.';


--
-- Name: COLUMN yield_event.amount; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.yield_event.amount IS 'amount of yield (bales, kg or whatever)';


--
-- Name: yield_type; Type: TABLE; Schema: cropping; Owner: -
--

CREATE TABLE cropping.yield_type (
    id integer DEFAULT nextval('cropping.cropping_id_seq'::regclass) NOT NULL,
    name character varying NOT NULL,
    mass numeric,
    comment character varying
);


--
-- Name: COLUMN yield_type.mass; Type: COMMENT; Schema: cropping; Owner: -
--

COMMENT ON COLUMN cropping.yield_type.mass IS 'kg';


--
-- Name: document; Type: TABLE; Schema: documents; Owner: -
--

CREATE TABLE documents.document (
    id integer NOT NULL,
    file_name character varying,
    display_name character varying,
    management boolean DEFAULT false NOT NULL,
    machine_id integer,
    active boolean DEFAULT true NOT NULL
);


--
-- Name: COLUMN document.management; Type: COMMENT; Schema: documents; Owner: -
--

COMMENT ON COLUMN documents.document.management IS 'when true only management group sees the file.';


--
-- Name: COLUMN document.machine_id; Type: COMMENT; Schema: documents; Owner: -
--

COMMENT ON COLUMN documents.document.machine_id IS 'when document related to specific machine';


--
-- Name: COLUMN document.active; Type: COMMENT; Schema: documents; Owner: -
--

COMMENT ON COLUMN documents.document.active IS 'true when still in use';


--
-- Name: document_id_seq; Type: SEQUENCE; Schema: documents; Owner: -
--

CREATE SEQUENCE documents.document_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: document_id_seq; Type: SEQUENCE OWNED BY; Schema: documents; Owner: -
--

ALTER SEQUENCE documents.document_id_seq OWNED BY documents.document.id;


--
-- Name: coverage; Type: TABLE; Schema: gis; Owner: -
--

CREATE TABLE gis.coverage (
    gid integer NOT NULL,
    version character varying(8),
    gps_status smallint,
    status_txt character varying(8),
    swath integer,
    height double precision,
    dateclosed date,
    timeclosed character varying(10),
    appldrate double precision,
    material character varying(30),
    materialid integer,
    speed double precision,
    the_geom gis.geometry,
    moisture double precision,
    event character varying
);


--
-- Name: coverage_gid_seq; Type: SEQUENCE; Schema: gis; Owner: -
--

CREATE SEQUENCE gis.coverage_gid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: coverage_gid_seq; Type: SEQUENCE OWNED BY; Schema: gis; Owner: -
--

ALTER SEQUENCE gis.coverage_gid_seq OWNED BY gis.coverage.gid;


--
-- Name: roadstable; Type: TABLE; Schema: gis; Owner: -
--

CREATE TABLE gis.roadstable (
    gid integer NOT NULL,
    version character varying(8),
    gps_status integer,
    status_txt character varying(8),
    swath integer,
    height double precision,
    dateclosed date,
    timeclosed character varying(10),
    appldrate double precision,
    material character varying(30),
    materialid integer,
    speed double precision
);


--
-- Name: roadstable_gid_seq; Type: SEQUENCE; Schema: gis; Owner: -
--

CREATE SEQUENCE gis.roadstable_gid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: roadstable_gid_seq; Type: SEQUENCE OWNED BY; Schema: gis; Owner: -
--

ALTER SEQUENCE gis.roadstable_gid_seq OWNED BY gis.roadstable.gid;


--
-- Name: summary; Type: TABLE; Schema: gis; Owner: -
--

CREATE TABLE gis.summary (
    id integer NOT NULL,
    client_name text,
    farm_name text,
    field_name text,
    event_name text,
    event_created timestamp without time zone,
    summary_created timestamp without time zone,
    field_area numeric,
    productive_area numeric,
    total_time time without time zone,
    coverage_area numeric,
    implement_width numeric,
    start_time timestamp without time zone,
    end_time timestamp without time zone,
    coverage_time time without time zone,
    geo_point gis.geometry
);


--
-- Name: TABLE summary; Type: COMMENT; Schema: gis; Owner: -
--

COMMENT ON TABLE gis.summary IS 'gps summary info from rtf.';


--
-- Name: summary_id_seq; Type: SEQUENCE; Schema: gis; Owner: -
--

CREATE SEQUENCE gis.summary_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: summary_id_seq; Type: SEQUENCE OWNED BY; Schema: gis; Owner: -
--

ALTER SEQUENCE gis.summary_id_seq OWNED BY gis.summary.id;


--
-- Name: swaths; Type: TABLE; Schema: gis; Owner: -
--

CREATE TABLE gis.swaths (
    gid integer NOT NULL,
    date date,
    "time" character varying(10),
    version character varying(8),
    id smallint,
    name character varying(48),
    length double precision,
    dist1 double precision,
    dist2 double precision
);


--
-- Name: swaths_gid_seq; Type: SEQUENCE; Schema: gis; Owner: -
--

CREATE SEQUENCE gis.swaths_gid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: swaths_gid_seq; Type: SEQUENCE OWNED BY; Schema: gis; Owner: -
--

ALTER SEQUENCE gis.swaths_gid_seq OWNED BY gis.swaths.gid;


--
-- Name: overtime; Type: TABLE; Schema: hr; Owner: -
--

CREATE TABLE hr.overtime (
    id integer NOT NULL,
    event_date date NOT NULL,
    overtime_userid character(32) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    comment character varying,
    hours numeric NOT NULL
);


--
-- Name: TABLE overtime; Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON TABLE hr.overtime IS 'tracks employee overtime';


--
-- Name: COLUMN overtime.userid; Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON COLUMN hr.overtime.userid IS 'the user who entered the overtime';


--
-- Name: COLUMN overtime.hours; Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON COLUMN hr.overtime.hours IS 'amount of overtime';


--
-- Name: overtime_id_seq; Type: SEQUENCE; Schema: hr; Owner: -
--

CREATE SEQUENCE hr.overtime_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: overtime_id_seq; Type: SEQUENCE OWNED BY; Schema: hr; Owner: -
--

ALTER SEQUENCE hr.overtime_id_seq OWNED BY hr.overtime.id;


--
-- Name: time_sheet; Type: TABLE; Schema: hr; Owner: -
--

CREATE TABLE hr.time_sheet (
    id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    time_worked numeric NOT NULL,
    time_banked numeric NOT NULL,
    deleted boolean NOT NULL,
    explanation character varying,
    shift_type character varying,
    explanation_overtime text
);


--
-- Name: TABLE time_sheet; Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON TABLE hr.time_sheet IS 'keeps track of employee hours.';


--
-- Name: COLUMN time_sheet.time_worked; Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON COLUMN hr.time_sheet.time_worked IS 'amount of hours actually worked.';


--
-- Name: COLUMN time_sheet.time_banked; Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON COLUMN hr.time_sheet.time_banked IS 'amount of positive or neg hours banked.';


--
-- Name: COLUMN time_sheet.deleted; Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON COLUMN hr.time_sheet.deleted IS 'when true record is deleted.';


--
-- Name: COLUMN time_sheet.explanation; Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON COLUMN hr.time_sheet.explanation IS 'text to explain, ie day off';


--
-- Name: COLUMN time_sheet.explanation_overtime; Type: COMMENT; Schema: hr; Owner: -
--

COMMENT ON COLUMN hr.time_sheet.explanation_overtime IS 'employee reason for overtime';


--
-- Name: time_bank_id_seq; Type: SEQUENCE; Schema: hr; Owner: -
--

CREATE SEQUENCE hr.time_bank_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: time_bank_id_seq; Type: SEQUENCE OWNED BY; Schema: hr; Owner: -
--

ALTER SEQUENCE hr.time_bank_id_seq OWNED BY hr.time_sheet.id;


--
-- Name: time_sheet_view; Type: VIEW; Schema: hr; Owner: -
--

CREATE VIEW hr.time_sheet_view AS
 SELECT time_sheet.id,
    time_sheet.event_time,
    time_sheet.userid,
    time_sheet.create_time,
    time_sheet.update_time,
    time_sheet.time_worked,
    time_sheet.explanation,
    time_sheet.shift_type,
    time_sheet.time_banked,
    (time_sheet.time_worked - time_sheet.time_banked) AS total_time,
    time_sheet.explanation_overtime,
    time_sheet.deleted
   FROM hr.time_sheet;


--
-- Name: ajax; Type: TABLE; Schema: intwebsite; Owner: -
--

CREATE TABLE intwebsite.ajax (
    id integer NOT NULL,
    filename character varying NOT NULL,
    filesubdir character varying NOT NULL
);


--
-- Name: TABLE ajax; Type: COMMENT; Schema: intwebsite; Owner: -
--

COMMENT ON TABLE intwebsite.ajax IS 'Used for api to access ajax calls in other parts of the site. Possible security hole.';


--
-- Name: ajax_id_seq; Type: SEQUENCE; Schema: intwebsite; Owner: -
--

CREATE SEQUENCE intwebsite.ajax_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: ajax_id_seq; Type: SEQUENCE OWNED BY; Schema: intwebsite; Owner: -
--

ALTER SEQUENCE intwebsite.ajax_id_seq OWNED BY intwebsite.ajax.id;


--
-- Name: ajax_security; Type: TABLE; Schema: intwebsite; Owner: -
--

CREATE TABLE intwebsite.ajax_security (
    id integer NOT NULL,
    ajax_id integer NOT NULL,
    "group" character(64) NOT NULL
);


--
-- Name: TABLE ajax_security; Type: COMMENT; Schema: intwebsite; Owner: -
--

COMMENT ON TABLE intwebsite.ajax_security IS 'is ajax resource accessible by a certain group';


--
-- Name: ajax_security_id_seq; Type: SEQUENCE; Schema: intwebsite; Owner: -
--

CREATE SEQUENCE intwebsite.ajax_security_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: ajax_security_id_seq; Type: SEQUENCE OWNED BY; Schema: intwebsite; Owner: -
--

ALTER SEQUENCE intwebsite.ajax_security_id_seq OWNED BY intwebsite.ajax_security.id;


--
-- Name: page; Type: TABLE; Schema: intwebsite; Owner: -
--

CREATE TABLE intwebsite.page (
    pageid integer NOT NULL,
    filename character varying(256) NOT NULL,
    title character varying(256) NOT NULL,
    filesubdir character varying(256),
    parent_id integer,
    rank integer,
    auto_refresh boolean DEFAULT true NOT NULL,
    active boolean DEFAULT true NOT NULL
);


--
-- Name: TABLE page; Type: COMMENT; Schema: intwebsite; Owner: -
--

COMMENT ON TABLE intwebsite.page IS 'main table for a page on the website';


--
-- Name: COLUMN page.rank; Type: COMMENT; Schema: intwebsite; Owner: -
--

COMMENT ON COLUMN intwebsite.page.rank IS 'rank between brothers and sisters';


--
-- Name: COLUMN page.active; Type: COMMENT; Schema: intwebsite; Owner: -
--

COMMENT ON COLUMN intwebsite.page.active IS 'page active or not.';


--
-- Name: page_pageid_seq; Type: SEQUENCE; Schema: intwebsite; Owner: -
--

CREATE SEQUENCE intwebsite.page_pageid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: page_pageid_seq; Type: SEQUENCE OWNED BY; Schema: intwebsite; Owner: -
--

ALTER SEQUENCE intwebsite.page_pageid_seq OWNED BY intwebsite.page.pageid;


--
-- Name: page_security; Type: TABLE; Schema: intwebsite; Owner: -
--

CREATE TABLE intwebsite.page_security (
    id integer NOT NULL,
    pageid integer NOT NULL,
    "group" character(64)
);


--
-- Name: TABLE page_security; Type: COMMENT; Schema: intwebsite; Owner: -
--

COMMENT ON TABLE intwebsite.page_security IS 'Who is allowed to access specific pages.';


--
-- Name: page_security_id_seq; Type: SEQUENCE; Schema: intwebsite; Owner: -
--

CREATE SEQUENCE intwebsite.page_security_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: page_security_id_seq; Type: SEQUENCE OWNED BY; Schema: intwebsite; Owner: -
--

ALTER SEQUENCE intwebsite.page_security_id_seq OWNED BY intwebsite.page_security.id;


--
-- Name: sse_id_seq; Type: SEQUENCE; Schema: intwebsite; Owner: -
--

CREATE SEQUENCE intwebsite.sse_id_seq
    START WITH 20000
    INCREMENT BY 1
    MINVALUE 20000
    NO MAXVALUE
    CACHE 1;


--
-- Name: sse; Type: TABLE; Schema: intwebsite; Owner: -
--

CREATE TABLE intwebsite.sse (
    id integer DEFAULT nextval('intwebsite.sse_id_seq'::regclass) NOT NULL,
    filename character varying NOT NULL,
    filesubdir character varying NOT NULL,
    name text NOT NULL
);


--
-- Name: COLUMN sse.name; Type: COMMENT; Schema: intwebsite; Owner: -
--

COMMENT ON COLUMN intwebsite.sse.name IS 'name of what it does.';


--
-- Name: sse_security_id_seq; Type: SEQUENCE; Schema: intwebsite; Owner: -
--

CREATE SEQUENCE intwebsite.sse_security_id_seq
    START WITH 2000
    INCREMENT BY 1
    MINVALUE 2000
    NO MAXVALUE
    CACHE 1;


--
-- Name: sse_security; Type: TABLE; Schema: intwebsite; Owner: -
--

CREATE TABLE intwebsite.sse_security (
    id integer DEFAULT nextval('intwebsite.sse_security_id_seq'::regclass) NOT NULL,
    sse_id integer NOT NULL,
    "group" character(64) NOT NULL
);


--
-- Name: comment; Type: TABLE; Schema: machinery; Owner: -
--

CREATE TABLE machinery.comment (
    id integer NOT NULL,
    machine_id integer NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment text NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: comment_id_seq; Type: SEQUENCE; Schema: machinery; Owner: -
--

CREATE SEQUENCE machinery.comment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: comment_id_seq; Type: SEQUENCE OWNED BY; Schema: machinery; Owner: -
--

ALTER SEQUENCE machinery.comment_id_seq OWNED BY machinery.comment.id;


--
-- Name: hours_log; Type: TABLE; Schema: machinery; Owner: -
--

CREATE TABLE machinery.hours_log (
    machine_id integer NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    hours interval NOT NULL,
    id integer NOT NULL
);


--
-- Name: TABLE hours_log; Type: COMMENT; Schema: machinery; Owner: -
--

COMMENT ON TABLE machinery.hours_log IS 'tracks hours on machinery';


--
-- Name: hours_log_id_seq; Type: SEQUENCE; Schema: machinery; Owner: -
--

CREATE SEQUENCE machinery.hours_log_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: hours_log_id_seq; Type: SEQUENCE OWNED BY; Schema: machinery; Owner: -
--

ALTER SEQUENCE machinery.hours_log_id_seq OWNED BY machinery.hours_log.id;


--
-- Name: machine; Type: TABLE; Schema: machinery; Owner: -
--

CREATE TABLE machinery.machine (
    id integer NOT NULL,
    name text,
    active boolean DEFAULT true NOT NULL,
    serial_num character varying,
    hours_log boolean DEFAULT true NOT NULL
);


--
-- Name: TABLE machine; Type: COMMENT; Schema: machinery; Owner: -
--

COMMENT ON TABLE machinery.machine IS 'stores type of machines';


--
-- Name: COLUMN machine.hours_log; Type: COMMENT; Schema: machinery; Owner: -
--

COMMENT ON COLUMN machinery.machine.hours_log IS 'does it have odometer, etc?';


--
-- Name: machine_id_seq; Type: SEQUENCE; Schema: machinery; Owner: -
--

CREATE SEQUENCE machinery.machine_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: machine_id_seq; Type: SEQUENCE OWNED BY; Schema: machinery; Owner: -
--

ALTER SEQUENCE machinery.machine_id_seq OWNED BY machinery.machine.id;


--
-- Name: service_administered; Type: TABLE; Schema: machinery; Owner: -
--

CREATE TABLE machinery.service_administered (
    service_item_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    comment text,
    hours_log_id integer,
    id integer NOT NULL
);


--
-- Name: COLUMN service_administered.comment; Type: COMMENT; Schema: machinery; Owner: -
--

COMMENT ON COLUMN machinery.service_administered.comment IS 'optional';


--
-- Name: COLUMN service_administered.hours_log_id; Type: COMMENT; Schema: machinery; Owner: -
--

COMMENT ON COLUMN machinery.service_administered.hours_log_id IS 'within 1 day of service.';


--
-- Name: service_administered_id_seq; Type: SEQUENCE; Schema: machinery; Owner: -
--

CREATE SEQUENCE machinery.service_administered_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: service_administered_id_seq; Type: SEQUENCE OWNED BY; Schema: machinery; Owner: -
--

ALTER SEQUENCE machinery.service_administered_id_seq OWNED BY machinery.service_administered.id;


--
-- Name: service_item; Type: TABLE; Schema: machinery; Owner: -
--

CREATE TABLE machinery.service_item (
    id integer NOT NULL,
    machine_id integer NOT NULL,
    name text NOT NULL,
    part_num text,
    hours interval NOT NULL,
    napa_part_num character varying
);


--
-- Name: TABLE service_item; Type: COMMENT; Schema: machinery; Owner: -
--

COMMENT ON TABLE machinery.service_item IS 'for each machine this is the different things that need serviced.';


--
-- Name: COLUMN service_item.part_num; Type: COMMENT; Schema: machinery; Owner: -
--

COMMENT ON COLUMN machinery.service_item.part_num IS 'part number';


--
-- Name: COLUMN service_item.hours; Type: COMMENT; Schema: machinery; Owner: -
--

COMMENT ON COLUMN machinery.service_item.hours IS 'hours between services';


--
-- Name: service_item_id_seq; Type: SEQUENCE; Schema: machinery; Owner: -
--

CREATE SEQUENCE machinery.service_item_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: service_item_id_seq; Type: SEQUENCE OWNED BY; Schema: machinery; Owner: -
--

ALTER SEQUENCE machinery.service_item_id_seq OWNED BY machinery.service_item.id;


--
-- Name: chat_text; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.chat_text (
    create_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    text text,
    update_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE chat_text; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON TABLE misc.chat_text IS 'holds text from direct chat';


--
-- Name: cqm_record17; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.cqm_record17 (
    id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    problem text NOT NULL,
    corrective_action text NOT NULL,
    userid character(32) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL
);


--
-- Name: TABLE cqm_record17; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON TABLE misc.cqm_record17 IS 'Holds Canadian Quality Milk (CQM) record 17.';


--
-- Name: cqm_record17_id_seq; Type: SEQUENCE; Schema: misc; Owner: -
--

CREATE SEQUENCE misc.cqm_record17_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: cqm_record17_id_seq; Type: SEQUENCE OWNED BY; Schema: misc; Owner: -
--

ALTER SEQUENCE misc.cqm_record17_id_seq OWNED BY misc.cqm_record17.id;


--
-- Name: cqm_record13; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.cqm_record13 (
    id integer DEFAULT nextval('misc.cqm_record17_id_seq'::regclass) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    corrective_action text NOT NULL,
    userid character(32) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    tanka_paddle boolean,
    tanka_dipstick boolean,
    tanka_surface boolean,
    tanka_outlet boolean,
    tanka_valve boolean,
    tanka_gaskets boolean,
    tankb_paddle boolean,
    tankb_dipstick boolean,
    tankb_surface boolean,
    tankb_outlet boolean,
    tankb_valve boolean,
    tankb_gaskets boolean,
    parlor_receiverjar boolean,
    parlor_pipelineinlets boolean,
    parlor_inflations boolean,
    parlor_milkhoses boolean,
    parlor_claws boolean,
    parlor_meters boolean,
    parlor_gaskets boolean,
    parlor_filtercoil boolean,
    parlor_buckets boolean,
    parlor_sanitarytrap boolean,
    parlor_hotwatertemp boolean
);


--
-- Name: cqm_record9; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.cqm_record9 (
    id integer DEFAULT nextval('misc.cqm_record17_id_seq'::regclass) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    name text NOT NULL,
    label text NOT NULL,
    approved_for_dairy boolean DEFAULT false NOT NULL,
    stored_according_label boolean DEFAULT false NOT NULL,
    "DIN" character(8)
);


--
-- Name: COLUMN cqm_record9."DIN"; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON COLUMN misc.cqm_record9."DIN" IS 'Chemical DIN number';


--
-- Name: main; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.main (
    generate_dates date NOT NULL
);


--
-- Name: purchase_suppliers; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.purchase_suppliers (
    name character varying NOT NULL,
    active boolean DEFAULT true NOT NULL
);


--
-- Name: TABLE purchase_suppliers; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON TABLE misc.purchase_suppliers IS 'list of suppliers';


--
-- Name: purchases; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.purchases (
    id integer NOT NULL,
    requesting_event_time timestamp without time zone NOT NULL,
    requesting_userid character(32) NOT NULL,
    quantity integer NOT NULL,
    description character varying NOT NULL,
    supplier character varying,
    purchasing_userid character(32),
    purchasing_event_time timestamp without time zone,
    purchased boolean DEFAULT false,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    purchasing_comment character varying
);


--
-- Name: TABLE purchases; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON TABLE misc.purchases IS 'tracks purchases';


--
-- Name: COLUMN purchases.purchased; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON COLUMN misc.purchases.purchased IS 'true when item was purchased.';


--
-- Name: purchases_id_seq; Type: SEQUENCE; Schema: misc; Owner: -
--

CREATE SEQUENCE misc.purchases_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: purchases_id_seq; Type: SEQUENCE OWNED BY; Schema: misc; Owner: -
--

ALTER SEQUENCE misc.purchases_id_seq OWNED BY misc.purchases.id;


--
-- Name: sop; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.sop (
    id integer NOT NULL,
    title character varying NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    active boolean DEFAULT true NOT NULL
);


--
-- Name: TABLE sop; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON TABLE misc.sop IS 'holds sop info.';


--
-- Name: sop_content; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.sop_content (
    sop_id integer NOT NULL,
    content text NOT NULL,
    event_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE sop_content; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON TABLE misc.sop_content IS 'holds record through time of edits.';


--
-- Name: sop_id_seq; Type: SEQUENCE; Schema: misc; Owner: -
--

CREATE SEQUENCE misc.sop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: sop_id_seq; Type: SEQUENCE OWNED BY; Schema: misc; Owner: -
--

ALTER SEQUENCE misc.sop_id_seq OWNED BY misc.sop.id;


--
-- Name: sopcurr; Type: VIEW; Schema: misc; Owner: -
--

CREATE VIEW misc.sopcurr AS
 SELECT sop.id,
    sop.title,
    t.content,
    t.event_time,
    t.userid
   FROM (misc.sop_content t
     LEFT JOIN misc.sop ON ((sop.id = t.sop_id)))
  WHERE (t.event_time = ( SELECT max(sop_content.event_time) AS max
           FROM misc.sop_content
          WHERE (sop_content.sop_id = t.sop_id)));


--
-- Name: task; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.task (
    id integer NOT NULL,
    name character varying NOT NULL,
    time_to_complete interval NOT NULL,
    "interval" interval NOT NULL,
    description character varying,
    active boolean DEFAULT true NOT NULL,
    assigned_userid character(32)
);


--
-- Name: COLUMN task.time_to_complete; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON COLUMN misc.task.time_to_complete IS 'time interval';


--
-- Name: COLUMN task."interval"; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON COLUMN misc.task."interval" IS 'time interval';


--
-- Name: COLUMN task.assigned_userid; Type: COMMENT; Schema: misc; Owner: -
--

COMMENT ON COLUMN misc.task.assigned_userid IS 'which user the task is assigned too.';


--
-- Name: task_completed; Type: TABLE; Schema: misc; Owner: -
--

CREATE TABLE misc.task_completed (
    task_id integer NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    comment character varying,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL
);


--
-- Name: task_id_seq; Type: SEQUENCE; Schema: misc; Owner: -
--

CREATE SEQUENCE misc.task_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: task_id_seq; Type: SEQUENCE OWNED BY; Schema: misc; Owner: -
--

ALTER SEQUENCE misc.task_id_seq OWNED BY misc.task.id;


--
-- Name: taskcurr; Type: VIEW; Schema: misc; Owner: -
--

CREATE VIEW misc.taskcurr AS
 WITH temp_table AS (
         SELECT task.id,
            task.name,
            task.time_to_complete,
            task."interval",
            task.description,
            task.active,
            task.assigned_userid,
            ( SELECT max(task_completed.event_time) AS max
                   FROM misc.task_completed
                  WHERE (task_completed.task_id = task.id)) AS last_completion_event_time,
            ( SELECT task_completed.userid
                   FROM misc.task_completed
                  WHERE ((task_completed.task_id = task.id) AND (task_completed.event_time = ( SELECT max(task_completed_1.event_time) AS max
                           FROM misc.task_completed task_completed_1
                          WHERE (task_completed_1.task_id = task.id))))) AS last_completion_userid,
            ( SELECT task_completed.comment
                   FROM misc.task_completed
                  WHERE ((task_completed.task_id = task.id) AND (task_completed.event_time = ( SELECT max(task_completed_1.event_time) AS max
                           FROM misc.task_completed task_completed_1
                          WHERE (task_completed_1.task_id = task.id))))) AS last_completion_comment
           FROM misc.task
        )
 SELECT temp_table.id,
    temp_table.name,
    temp_table.time_to_complete,
    temp_table."interval",
    temp_table.description,
    temp_table.last_completion_event_time,
    temp_table.assigned_userid,
    temp_table.last_completion_userid,
    temp_table.last_completion_comment,
    (now() - ((temp_table.last_completion_event_time + temp_table."interval"))::timestamp with time zone) AS last_completion_elapsed_time
   FROM temp_table
  WHERE (temp_table.active = true);


--
-- Name: recipe; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.recipe (
    id integer NOT NULL,
    date timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    comment character varying,
    active boolean DEFAULT true NOT NULL,
    editable boolean DEFAULT false NOT NULL,
    comment2 character varying,
    parent_id numeric NOT NULL
);


--
-- Name: TABLE recipe; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.recipe IS 'DEPRECATED. holds recipe info';


--
-- Name: COLUMN recipe.comment; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.recipe.comment IS 'really NAME';


--
-- Name: COLUMN recipe.editable; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.recipe.editable IS 'only newly created reciped are editable';


--
-- Name: COLUMN recipe.comment2; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.recipe.comment2 IS 'comment to display at bottom of recipe.';


--
-- Name: COLUMN recipe.parent_id; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.recipe.parent_id IS 'holds a recipe considered to be the parent for this one';


--
-- Name: 3_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition."3_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: 3_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition."3_id_seq" OWNED BY nutrition.recipe.id;


--
-- Name: analysis_link; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.analysis_link (
    id integer NOT NULL,
    feedcurr_id text,
    feed_library_id integer NOT NULL,
    analysis_id integer NOT NULL
);


--
-- Name: TABLE analysis_link; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.analysis_link IS 'links book values and feed tests together to a specific item';


--
-- Name: COLUMN analysis_link.feedcurr_id; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.analysis_link.feedcurr_id IS 'can be null when no test is available, use book values.';


--
-- Name: analysis_link_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.analysis_link_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: analysis_link_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.analysis_link_id_seq OWNED BY nutrition.analysis_link.id;


--
-- Name: bag; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.bag (
    id integer NOT NULL,
    location character(32) NOT NULL,
    slot integer NOT NULL,
    diameter_foot numeric NOT NULL,
    event_time timestamp without time zone NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    comment text,
    userid character(32) NOT NULL
);


--
-- Name: TABLE bag; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.bag IS 'record forages and commodities in ag-bag.';


--
-- Name: COLUMN bag.location; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag.location IS 'always "North Silage Pad" for now.';


--
-- Name: COLUMN bag.slot; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag.slot IS 'which bag number.';


--
-- Name: COLUMN bag.diameter_foot; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag.diameter_foot IS 'always "10" for now.';


--
-- Name: COLUMN bag.event_time; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag.event_time IS 'just record date for now.';


--
-- Name: nutrition_sequence; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.nutrition_sequence
    START WITH 5000
    INCREMENT BY 1
    MINVALUE 5000
    NO MAXVALUE
    CACHE 1;


--
-- Name: bag_analysis; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.bag_analysis (
    id integer DEFAULT nextval('nutrition.nutrition_sequence'::regclass) NOT NULL,
    bag_id integer,
    footage integer,
    sample_number text NOT NULL,
    sample_date timestamp without time zone,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    report_file bytea,
    lab_name text,
    raw_report jsonb NOT NULL,
    nrc2001_template jsonb,
    info_template jsonb
);


--
-- Name: TABLE bag_analysis; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.bag_analysis IS 'stores forage tests in json';


--
-- Name: COLUMN bag_analysis.raw_report; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag_analysis.raw_report IS 'parsed from report using report names for things or old shurgain template names pigeon holed to fit.';


--
-- Name: COLUMN bag_analysis.nrc2001_template; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag_analysis.nrc2001_template IS 'report data into NRC 2001 template. ';


--
-- Name: COLUMN bag_analysis.info_template; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag_analysis.info_template IS 'used by INT site for general info, ie bags pages';


--
-- Name: bag_comment; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.bag_comment (
    id integer NOT NULL,
    bag_id integer NOT NULL,
    footage numeric NOT NULL,
    comment character varying(256) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE bag_comment; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.bag_comment IS 'records a comment at a specifc footage on bag.';


--
-- Name: bag_comment_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.bag_comment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: bag_comment_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.bag_comment_id_seq OWNED BY nutrition.bag_comment.id;


--
-- Name: bag_consumption; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.bag_consumption (
    id integer NOT NULL,
    bag_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    footage numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    direction character(7) DEFAULT 'forward'::bpchar NOT NULL
);


--
-- Name: TABLE bag_consumption; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.bag_consumption IS 'tracks how much of a bag is fed out';


--
-- Name: COLUMN bag_consumption.direction; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag_consumption.direction IS 'forward or reverse';


--
-- Name: bag_consumption_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.bag_consumption_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: bag_consumption_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.bag_consumption_id_seq OWNED BY nutrition.bag_consumption.id;


--
-- Name: bag_cost; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.bag_cost (
    id integer NOT NULL,
    bag_id integer NOT NULL,
    footage_start numeric NOT NULL,
    footage_finish numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    cost numeric NOT NULL
);


--
-- Name: TABLE bag_cost; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.bag_cost IS 'tracks how much things cost in the bag per tonne of DM. Not by the foot.';


--
-- Name: COLUMN bag_cost.cost; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag_cost.cost IS 'cost per DM tonne';


--
-- Name: bag_feed; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.bag_feed (
    id integer NOT NULL,
    bag_id integer NOT NULL,
    feed_id integer NOT NULL,
    footage_start numeric NOT NULL,
    footage_finish numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE bag_feed; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.bag_feed IS 'what feed and where it is in the bag.';


--
-- Name: bag_current; Type: VIEW; Schema: nutrition; Owner: -
--

CREATE VIEW nutrition.bag_current AS
 SELECT bag.id,
    bag.location,
    bag.slot,
    bag.diameter_foot,
    bag.event_time,
    bag.create_time,
    bag.update_time,
    bag.comment,
    bag.userid
   FROM nutrition.bag
  WHERE (((( SELECT max(bag_consumption.footage) AS max_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.bag_id = bag.id) AND (bag_consumption.direction = 'forward'::bpchar))) < ( SELECT max(bag_feed.footage_finish) AS max
           FROM nutrition.bag_feed
          WHERE (bag_feed.bag_id = bag.id))) OR (( SELECT min(bag_consumption.footage) AS min_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.bag_id = bag.id) AND (bag_consumption.direction = 'reverse'::bpchar))) > ( SELECT min(bag_feed.footage_start) AS min
           FROM nutrition.bag_feed
          WHERE (bag_feed.bag_id = bag.id))) OR (( SELECT max(bag_consumption.footage) AS max
           FROM nutrition.bag_consumption
          WHERE (bag_consumption.bag_id = bag.id)) IS NULL)) AND ((( SELECT min(bag_consumption.footage) AS min_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.direction = 'reverse'::bpchar) AND (bag_consumption.bag_id = bag.id))) = ( SELECT max(bag_consumption.footage) AS max_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.direction = 'forward'::bpchar) AND (bag_consumption.bag_id = bag.id)))) IS NOT TRUE));


--
-- Name: VIEW bag_current; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON VIEW nutrition.bag_current IS 'current bags on the pad, open or closed, supports both directions.';


--
-- Name: bag_current_open; Type: VIEW; Schema: nutrition; Owner: -
--

CREATE VIEW nutrition.bag_current_open AS
 SELECT bag.id,
    (bag.id || '|forward'::text) AS id2,
    bag.location,
    bag.slot,
    bag.diameter_foot,
    bag.event_time,
    bag.create_time,
    bag.update_time,
    bag.comment,
    bag.userid,
    'forward'::text AS direction,
    nutrition.bag_current_consumption_footage(bag.id, 'forward'::text) AS footage,
    nutrition.bag_feed_name_at_footage(bag.id, nutrition.bag_current_consumption_footage(bag.id, 'forward'::text)) AS current_feed
   FROM nutrition.bag
  WHERE ((( SELECT max(bag_consumption.footage) AS max_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.direction = 'forward'::bpchar) AND (bag_consumption.bag_id = bag.id))) < ( SELECT max(bag_feed.footage_finish) AS max
           FROM nutrition.bag_feed
          WHERE (bag_feed.bag_id = bag.id))) AND ((( SELECT min(bag_consumption.footage) AS min_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.direction = 'reverse'::bpchar) AND (bag_consumption.bag_id = bag.id))) = ( SELECT max(bag_consumption.footage) AS max_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.direction = 'forward'::bpchar) AND (bag_consumption.bag_id = bag.id)))) IS NOT TRUE))
UNION
 SELECT bag.id,
    (bag.id || '|reverse'::text) AS id2,
    bag.location,
    bag.slot,
    bag.diameter_foot,
    bag.event_time,
    bag.create_time,
    bag.update_time,
    bag.comment,
    bag.userid,
    'reverse'::text AS direction,
    nutrition.bag_current_consumption_footage(bag.id, 'reverse'::text) AS footage,
    nutrition.bag_feed_name_at_footage(bag.id, nutrition.bag_current_consumption_footage(bag.id, 'reverse'::text)) AS current_feed
   FROM nutrition.bag
  WHERE ((( SELECT min(bag_consumption.footage) AS min_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.direction = 'reverse'::bpchar) AND (bag_consumption.bag_id = bag.id))) > ( SELECT min(bag_feed.footage_start) AS min
           FROM nutrition.bag_feed
          WHERE (bag_feed.bag_id = bag.id))) AND ((( SELECT min(bag_consumption.footage) AS min_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.direction = 'reverse'::bpchar) AND (bag_consumption.bag_id = bag.id))) = ( SELECT max(bag_consumption.footage) AS max_consumed_footage
           FROM nutrition.bag_consumption
          WHERE ((bag_consumption.direction = 'forward'::bpchar) AND (bag_consumption.bag_id = bag.id)))) IS NOT TRUE));


--
-- Name: VIEW bag_current_open; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON VIEW nutrition.bag_current_open IS 'Shows which bags are currently open, support bags open on both ends.';


--
-- Name: bag_ensile_date; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.bag_ensile_date (
    id integer NOT NULL,
    bag_id integer NOT NULL,
    event_time date NOT NULL,
    footage numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: TABLE bag_ensile_date; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.bag_ensile_date IS 'Date the the crop was en-siled in the bag at that footage.';


--
-- Name: bag_ensile_date_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.bag_ensile_date_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: bag_ensile_date_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.bag_ensile_date_id_seq OWNED BY nutrition.bag_ensile_date.id;


--
-- Name: bag_ensiled_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.bag_ensiled_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: bag_ensiled_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.bag_ensiled_id_seq OWNED BY nutrition.bag.id;


--
-- Name: bag_feed_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.bag_feed_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: bag_feed_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.bag_feed_id_seq OWNED BY nutrition.bag_feed.id;


--
-- Name: bag_field; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.bag_field (
    id integer NOT NULL,
    bag_id integer NOT NULL,
    field_id integer NOT NULL,
    footage_start numeric NOT NULL,
    footage_finish numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    CONSTRAINT footage_check CHECK ((footage_start < footage_finish))
);


--
-- Name: TABLE bag_field; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.bag_field IS 'where in a bag crops from specific fields are located.';


--
-- Name: bag_field_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.bag_field_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: bag_field_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.bag_field_id_seq OWNED BY nutrition.bag_field.id;


--
-- Name: bag_moisture_test; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.bag_moisture_test (
    id integer NOT NULL,
    bag_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    footage numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    percent_dry_matter numeric NOT NULL
);


--
-- Name: TABLE bag_moisture_test; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.bag_moisture_test IS 'records moisture tests performed on a bag.';


--
-- Name: COLUMN bag_moisture_test.percent_dry_matter; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.bag_moisture_test.percent_dry_matter IS 'percent dry matter, not percent of moisture';


--
-- Name: bag_moisture_test_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.bag_moisture_test_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: bag_moisture_test_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.bag_moisture_test_id_seq OWNED BY nutrition.bag_moisture_test.id;


--
-- Name: feed_type; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.feed_type (
    id integer NOT NULL,
    name character varying NOT NULL,
    comment text,
    colour_code character(6) NOT NULL,
    default_rounding_accuracy numeric DEFAULT 10 NOT NULL,
    concentrate boolean DEFAULT false NOT NULL
);


--
-- Name: TABLE feed_type; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.feed_type IS 'list of feeds';


--
-- Name: COLUMN feed_type.default_rounding_accuracy; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.feed_type.default_rounding_accuracy IS 'what place to round it to, can be customized later.';


--
-- Name: COLUMN feed_type.concentrate; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.feed_type.concentrate IS 'true when concentrate';


--
-- Name: bag_with_current_feed; Type: VIEW; Schema: nutrition; Owner: -
--

CREATE VIEW nutrition.bag_with_current_feed AS
 WITH temp_table AS (
         SELECT bag.id,
            bag.location,
            bag.slot,
            bag.diameter_foot,
            bag.event_time,
            bag.create_time,
            bag.update_time,
            bag.comment,
            bag.userid,
            feed.name AS current_feed,
            bag_feed.feed_id AS current_feed_id
           FROM ((nutrition.bag
             LEFT JOIN nutrition.bag_feed ON ((bag_feed.bag_id = bag.id)))
             LEFT JOIN nutrition.feed_type feed ON ((bag_feed.feed_id = feed.id)))
          WHERE ((bag_feed.bag_id = bag.id) AND (bag_feed.footage_start = ( SELECT max(fff.footage_start) AS footage_temp
                   FROM ( SELECT bag_feed_1.footage_start
                           FROM nutrition.bag_feed bag_feed_1
                          WHERE ((bag_feed_1.bag_id = bag.id) AND (bag_feed_1.footage_start <= ( SELECT max(bag_consumption.footage) AS max
                                   FROM nutrition.bag_consumption
                                  WHERE (bag_consumption.bag_id = bag.id))) AND (bag_feed_1.footage_finish >= ( SELECT max(bag_consumption.footage) AS max
                                   FROM nutrition.bag_consumption
                                  WHERE (bag_consumption.bag_id = bag.id))))
                        UNION
                         SELECT bag_feed_1.footage_start
                           FROM nutrition.bag_feed bag_feed_1
                          WHERE ((bag_feed_1.bag_id = bag.id) AND (bag_feed_1.footage_start = ( SELECT min(bag_feed_2.footage_start) AS min
                                   FROM nutrition.bag_feed bag_feed_2
                                  WHERE (bag_feed_2.bag_id = bag.id))))) fff)))
        ), temp_table2 AS (
         SELECT temp_table.id,
            temp_table.location,
            temp_table.slot,
            temp_table.diameter_foot,
            temp_table.event_time,
            temp_table.create_time,
            temp_table.update_time,
            temp_table.comment,
            temp_table.userid,
            temp_table.current_feed,
            temp_table.current_feed_id,
            ( SELECT max(bag_consumption.footage) AS max
                   FROM nutrition.bag_consumption
                  WHERE (bag_consumption.bag_id = temp_table.id)) AS current_footage
           FROM temp_table
        )
 SELECT temp_table2.id,
    temp_table2.location,
    temp_table2.slot,
    temp_table2.diameter_foot,
    temp_table2.event_time,
    temp_table2.create_time,
    temp_table2.update_time,
    temp_table2.comment,
    temp_table2.userid,
    temp_table2.current_feed,
    temp_table2.current_feed_id,
    temp_table2.current_footage,
    ( SELECT bag_cost.cost
           FROM nutrition.bag_cost
          WHERE ((bag_cost.bag_id = temp_table2.id) AND (bag_cost.footage_start <= temp_table2.current_footage) AND (bag_cost.footage_finish >= temp_table2.current_footage))
         LIMIT 1) AS cost_at_current_footage
   FROM temp_table2
  WHERE (temp_table2.current_footage < ( SELECT max(bag_feed.footage_finish) AS max
           FROM nutrition.bag_feed
          WHERE (bag_feed.bag_id = temp_table2.id)));


--
-- Name: cncps_feed; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.cncps_feed (
    source character varying,
    sourceid integer,
    feedname character varying,
    feedtype character varying,
    lastupdatedate date,
    editedby character varying,
    baseprice numeric,
    availableinmill boolean,
    density numeric,
    feeddatasource character varying,
    pwd character varying
);


--
-- Name: cncps_feed_chem; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.cncps_feed_chem (
    feedname character varying,
    source character varying,
    sourceid integer,
    dm numeric,
    conc numeric,
    forage numeric,
    cp numeric,
    sp numeric,
    npn numeric,
    adip numeric,
    ndip numeric,
    nfc numeric,
    acetic numeric,
    propionic numeric,
    butyric numeric,
    lactic numeric,
    otheroas numeric,
    sugar numeric,
    starch numeric,
    solfiber numeric,
    adf numeric,
    ndf numeric,
    pendf numeric,
    lignin numeric,
    ash numeric,
    l1_rup_1x numeric,
    met numeric,
    lys numeric,
    arg numeric,
    thr numeric,
    leu numeric,
    ile numeric,
    val numeric,
    his numeric,
    phe numeric,
    trp numeric,
    leucine numeric,
    isoleucine numeric,
    valine numeric,
    ee numeric,
    tfa_ee numeric,
    tfa_dm numeric,
    glycerol numeric,
    pigment numeric,
    c12_0 numeric,
    c14_0 numeric,
    c16_0 numeric,
    c16_1 numeric,
    c18_0 numeric,
    c18_1_trans numeric,
    c18_1_cis numeric,
    c18_2 numeric,
    c18_3 numeric,
    otherlipid numeric,
    ca numeric,
    p numeric,
    mg numeric,
    k numeric,
    s numeric,
    na numeric,
    cl numeric,
    fe numeric,
    zn numeric,
    cu numeric,
    mn numeric,
    se numeric,
    co numeric,
    i numeric,
    vita numeric,
    vitd numeric,
    vite numeric,
    monensin numeric,
    lasalocid numeric,
    decoquinate numeric,
    yeast numeric,
    beta_agonist numeric,
    virginiamycin numeric,
    aureomycin numeric,
    chlortetracycline numeric,
    oxytetracycline numeric,
    salinomycin numeric,
    zinc_bacitracin numeric,
    niacin numeric,
    biotin numeric,
    choline numeric,
    chromium numeric,
    organic_chromium numeric,
    organic_zinc numeric,
    organic_copper numeric,
    organic_manganese numeric,
    organic_selenium numeric,
    organic_cobalt numeric,
    enzymes numeric,
    toxin_binders numeric,
    flavor numeric,
    cho_a1_kd numeric,
    cho_a2_kd numeric,
    cho_a3_kd numeric,
    cho_a4_kd numeric,
    cho_b1_kd numeric,
    cho_b2_kd numeric,
    cho_b3_kd numeric,
    cho_c_kd numeric,
    prot_a_kd numeric,
    prot_b1_kd numeric,
    prot_b2_kd numeric,
    prot_b3_kd numeric,
    prot_c_kd numeric,
    lipolysisrate numeric,
    adjfactor numeric,
    cho_a1_id numeric,
    cho_a2_id numeric,
    cho_a3_id numeric,
    cho_a4_id numeric,
    cho_b1_id numeric,
    cho_b2_id numeric,
    cho_b3_id numeric,
    cho_c_id numeric,
    prot_a_id numeric,
    prot_b1_id numeric,
    prot_b2_id numeric,
    prot_b3_id numeric,
    prot_c_id numeric,
    fat_id numeric,
    c12_0_id numeric,
    c14_0_id numeric,
    c16_0_id numeric,
    c16_1_id numeric,
    c18_0_id numeric,
    c18_1_trans_id numeric,
    c18_1_cis_id numeric,
    c18_2_id numeric,
    c18_3_id numeric,
    otherlipid_id numeric,
    ca_bioavail numeric,
    p_bioavail numeric,
    mg_bioavail numeric,
    k_bioavail numeric,
    s_bioavail numeric,
    na_bioavail numeric,
    cl_bioavail numeric,
    fe_bioavail numeric,
    zn_bioavail numeric,
    cu_bioavail numeric,
    mn_bioavail numeric,
    se_bioavail numeric,
    co_bioavail numeric,
    i_bioavail numeric,
    vita_bioavail numeric,
    vitd_bioavail numeric,
    vite_bioavail numeric,
    inputted_dcad1 numeric,
    inputted_dcad2 numeric
);


--
-- Name: TABLE cncps_feed_chem; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.cncps_feed_chem IS 'passoword MVA612509';


--
-- Name: cost_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.cost_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: cost_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.cost_id_seq OWNED BY nutrition.bag_cost.id;


--
-- Name: feed; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.feed (
    id integer NOT NULL,
    feed_type_id integer NOT NULL,
    feed_location_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    active boolean DEFAULT true NOT NULL
);


--
-- Name: feed_cost; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.feed_cost (
    feed_id integer NOT NULL,
    event_time timestamp without time zone NOT NULL,
    cost numeric NOT NULL,
    userid character varying(32) NOT NULL
);


--
-- Name: COLUMN feed_cost.cost; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.feed_cost.cost IS 'per tonne of DM';


--
-- Name: feed_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.feed_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: feed_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.feed_id_seq OWNED BY nutrition.feed_type.id;


--
-- Name: feed_id_seq1; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.feed_id_seq1
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: feed_id_seq1; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.feed_id_seq1 OWNED BY nutrition.feed.id;


--
-- Name: feed_library_local; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.feed_library_local (
    "Feed Name" text NOT NULL,
    "Category" text,
    "Read Only" boolean,
    "IFN" text,
    "TDN (%DM)" numeric,
    "Energy Eq Class" text,
    "Forage Description" text,
    "PAF" numeric,
    "DE (Mcal/kg)" numeric,
    "DM (%AF)" numeric,
    "NDF (%DM)" numeric,
    "ADF (%DM)" numeric,
    "Lignin (%DM)" numeric,
    "CP (%DM)" numeric,
    "NDFIP (%DM)" text,
    "ADFIP (%DM)" text,
    "Prt-A (%CP)" numeric,
    "Prt-B (%CP)" numeric,
    "Prt-C (%CP)" numeric,
    "Kd (1/hr)" numeric,
    "RUP Digest (%)" numeric,
    "Fat (%DM)" numeric,
    "Ash (%DM)" numeric,
    "Ca (%DM)" numeric,
    "Ca-Bio (g/g)" numeric,
    "P (%DM)" numeric,
    "P-Bio (g/g)" numeric,
    "Mg (%DM)" numeric,
    "Mg-Bio (g/g)" numeric,
    "K (%DM)" numeric,
    "K-Bio (g/g)" numeric,
    "Na (%DM)" numeric,
    "Na-Bio (g/g)" numeric,
    "Cl (%DM)" numeric,
    "Cl-Bio (g/g)" numeric,
    "S (%DM)" numeric,
    "S-Bio (g/g)" numeric,
    "Co (mg/kg)" numeric,
    "Co-Bio (g/g)" numeric,
    "Cu (mg/kg)" numeric,
    "Cu-Bio (g/g)" numeric,
    "I (mg/kg)" numeric,
    "I-Bio (g/g)" numeric,
    "Fe (mg/kg)" numeric,
    "Fe-Bio (g/g)" numeric,
    "Mn (mg/kg)" numeric,
    "Mn-Bio (g/g)" numeric,
    "Se (mg/kg)" numeric,
    "Se-Bio (g/g)" numeric,
    "Zn (mg/kg)" numeric,
    "Zn-Bio (g/g)" numeric,
    "Arg (%CP)" numeric,
    "His (%CP)" numeric,
    "Ile (%CP)" numeric,
    "Leu (%CP)" numeric,
    "Lys (%CP)" numeric,
    "Met (%CP)" numeric,
    "Cys (%CP)" numeric,
    "Phe (%CP)" numeric,
    "Thr (%CP)" numeric,
    "Trp (%CP)" numeric,
    "Val (%CP)" numeric,
    "Vit-A (1000 IU/kg)" numeric,
    "Vit-D (1000 IU/kg)" numeric,
    "Vit-E (IU/kg)" numeric,
    "NFC Digest" numeric,
    "CP Digest" numeric,
    "NDF Digest" numeric,
    "Fat Digest" numeric,
    "cDM (%)" numeric,
    "cGE (Mcal/kg)" numeric,
    "cDE (Mcal/kg)" numeric,
    "cME (Mcal/kg)" numeric,
    "cME/cGE" numeric,
    "cNEm (Mcal/kg)" numeric,
    "cNEg (Mcal/kg)" numeric,
    "cCP (%DM)" numeric,
    "cDCP (%DM)" numeric,
    "cEE (%DM)" numeric,
    "cASH (%DM)" numeric
);


--
-- Name: feed_location; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.feed_location (
    id integer NOT NULL,
    location character varying NOT NULL,
    active boolean DEFAULT true NOT NULL
);


--
-- Name: TABLE feed_location; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.feed_location IS 'lists differnt places feed can be stored.';


--
-- Name: feed_location_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.feed_location_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: feed_location_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.feed_location_id_seq OWNED BY nutrition.feed_location.id;


--
-- Name: feed_moisture; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.feed_moisture (
    feed_id integer NOT NULL,
    date timestamp without time zone NOT NULL,
    dry_matter_percent numeric NOT NULL
);


--
-- Name: TABLE feed_moisture; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.feed_moisture IS 'tracks how the moisture changes with time for a particular feed.';


--
-- Name: feedcurr; Type: VIEW; Schema: nutrition; Owner: -
--

CREATE VIEW nutrition.feedcurr AS
 WITH temp_table AS (
         SELECT ('feed|'::text || feed.id) AS id,
            'feed'::text AS type,
            feed.id AS table_id,
            feed.active,
            feed.event_time AS feed_date,
            feed_location.location,
            feed_location.id AS feed_location_id,
            feed_type.name AS feed_type,
            feed_type.comment,
            feed_type.default_rounding_accuracy,
            feed_type.concentrate AS default_concentrate,
            ( SELECT max(feed_moisture.date) AS max
                   FROM nutrition.feed_moisture
                  WHERE (feed_moisture.feed_id = feed.id)) AS moisture_date,
            ( SELECT feed_moisture.dry_matter_percent AS moisture_percent
                   FROM nutrition.feed_moisture
                  WHERE ((feed_moisture.feed_id = feed.id) AND (feed_moisture.date = ( SELECT max(feed_moisture_1.date) AS max
                           FROM nutrition.feed_moisture feed_moisture_1
                          WHERE (feed_moisture_1.feed_id = feed.id))))) AS dry_matter_percent,
            ( SELECT feed_cost.cost
                   FROM nutrition.feed_cost
                  WHERE ((feed_cost.feed_id = feed.id) AND (feed_cost.event_time = ( SELECT max(feed_cost_1.event_time) AS max
                           FROM nutrition.feed_cost feed_cost_1
                          WHERE (feed_cost_1.feed_id = feed.id))))) AS cost,
            ( SELECT max(feed_cost.event_time) AS max
                   FROM nutrition.feed_cost
                  WHERE (feed_cost.feed_id = feed.id)) AS cost_date
           FROM ((nutrition.feed
             LEFT JOIN nutrition.feed_type ON ((feed_type.id = feed.feed_type_id)))
             LEFT JOIN nutrition.feed_location ON ((feed_location.id = feed.feed_location_id)))
        )
 SELECT temp_table.id,
    temp_table.type,
    temp_table.table_id,
    temp_table.feed_date,
    temp_table.location,
    temp_table.feed_type,
    temp_table.comment,
    temp_table.moisture_date,
    temp_table.dry_matter_percent,
    temp_table.cost,
    temp_table.default_rounding_accuracy,
    temp_table.default_concentrate AS concentrate
   FROM temp_table
  WHERE ((temp_table.feed_date = ( SELECT max(feed.event_time) AS max
           FROM nutrition.feed
          WHERE (feed.feed_location_id = temp_table.feed_location_id))) AND (temp_table.active = true))
UNION
( WITH tempx AS (
         SELECT bag_current_open.id,
            bag_current_open.id2,
            bag_current_open.location,
            bag_current_open.slot,
            bag_current_open.diameter_foot,
            bag_current_open.event_time,
            bag_current_open.create_time,
            bag_current_open.update_time,
            bag_current_open.comment,
            bag_current_open.userid,
            bag_current_open.direction,
            bag_current_open.footage,
            bag_current_open.current_feed,
            ( SELECT bag_moisture_test.percent_dry_matter
                   FROM nutrition.bag_moisture_test
                  WHERE (bag_moisture_test.id = nutrition.bag_closest_dry_matter_id(bag_current_open.id, bag_current_open.footage, bag_current_open.direction))) AS dry_matter_percent,
            ( SELECT bag_moisture_test.event_time
                   FROM nutrition.bag_moisture_test
                  WHERE (bag_moisture_test.id = nutrition.bag_closest_dry_matter_id(bag_current_open.id, bag_current_open.footage, bag_current_open.direction))) AS moisture_date,
            ( SELECT (((bag.location)::text || ' '::text) || bag.slot)
                   FROM nutrition.bag
                  WHERE (bag.id = bag_current_open.id)) AS location2,
            (((('bag'::text || '_'::text) || "substring"(bag_current_open.direction, 1, 1)) || '|'::text) || bag_current_open.id) AS feedcurr_id,
            feed_type.comment AS comment2,
            feed_type.default_rounding_accuracy,
            feed_type.concentrate AS default_concentrate
           FROM (nutrition.bag_current_open
             LEFT JOIN nutrition.feed_type ON (((feed_type.name)::text = bag_current_open.current_feed)))
        )
 SELECT tempx.feedcurr_id AS id,
    'bag'::text AS type,
    tempx.id AS table_id,
    tempx.event_time AS feed_date,
    tempx.location2 AS location,
    tempx.current_feed AS feed_type,
    tempx.comment2 AS comment,
    tempx.moisture_date,
    tempx.dry_matter_percent,
    nutrition.ingredient_cost(tempx.feedcurr_id, tempx.moisture_date) AS cost,
    tempx.default_rounding_accuracy,
    tempx.default_concentrate AS concentrate
   FROM tempx);


--
-- Name: mix; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.mix (
    id integer NOT NULL,
    recipe_id integer NOT NULL,
    location_id integer NOT NULL,
    date timestamp without time zone
);


--
-- Name: TABLE mix; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.mix IS 'DEPRECATED. number of mixes a day to multiple locations.';


--
-- Name: mix_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.mix_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: mix_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.mix_id_seq OWNED BY nutrition.mix.id;


--
-- Name: mix_modify; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.mix_modify (
    mix_id integer NOT NULL,
    date timestamp without time zone NOT NULL,
    modification_factor numeric NOT NULL
);


--
-- Name: TABLE mix_modify; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.mix_modify IS 'DEPRECATED. keeps track of modifying a mix';


--
-- Name: nrc_archive; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.nrc_archive (
    nrc_recipe_id integer NOT NULL,
    recipe_create_time timestamp without time zone NOT NULL,
    recipe jsonb NOT NULL
);


--
-- Name: TABLE nrc_archive; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.nrc_archive IS 'json log of archived recipes';


--
-- Name: COLUMN nrc_archive.recipe; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.nrc_archive.recipe IS 'json of recipe';


--
-- Name: nrc_recipe_param; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.nrc_recipe_param (
    nrc_recipe integer DEFAULT nextval('nutrition.nutrition_sequence'::regclass) NOT NULL,
    title text NOT NULL,
    comment text,
    status character varying(7) NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    create_userid character(32) NOT NULL,
    update_userid character(32) NOT NULL,
    CONSTRAINT status_check CHECK ((((status)::text = 'active'::text) OR ((status)::text = 'staging'::text) OR ((status)::text = 'archive'::text)))
);


--
-- Name: TABLE nrc_recipe_param; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.nrc_recipe_param IS 'PRIMARY TABLE: name of recipe, etc.';


--
-- Name: COLUMN nrc_recipe_param.status; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.nrc_recipe_param.status IS 'active,staging, or archive';


--
-- Name: COLUMN nrc_recipe_param.create_userid; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.nrc_recipe_param.create_userid IS 'who initially created userid';


--
-- Name: COLUMN nrc_recipe_param.update_userid; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.nrc_recipe_param.update_userid IS 'last update useried';


--
-- Name: nrc_archive_basic; Type: VIEW; Schema: nutrition; Owner: -
--

CREATE VIEW nutrition.nrc_archive_basic AS
 SELECT nrc_archive.nrc_recipe_id,
    nrc_recipe_param.create_time,
    (jsonb_array_elements(nrc_archive.recipe) -> 'location'::text) AS location,
    (jsonb_array_elements(nrc_archive.recipe) -> 'feed_type'::text) AS feed_type,
    (jsonb_array_elements(nrc_archive.recipe) -> 'kg_day_wet_mix'::text) AS kg_day_wet_mix,
    (((jsonb_array_elements(nrc_archive.recipe) ->> 'kg_day_dry'::text))::numeric * ((jsonb_array_elements(nrc_archive.recipe) ->> 'modified_mix_total_count'::text))::numeric) AS kg_day_dry_mix,
    (jsonb_array_elements(nrc_archive.recipe) -> 'location_id_arr'::text) AS location_id_arr,
    ((jsonb_array_elements(nrc_archive.recipe) ->> 'modified_mix_total_count'::text))::numeric AS modified_mix_total_count,
    (jsonb_array_elements(nrc_archive.recipe) -> 'location_mod_count_arr'::text) AS location_mod_count_arr
   FROM (nutrition.nrc_archive
     LEFT JOIN nutrition.nrc_recipe_param ON ((nrc_recipe_param.nrc_recipe = nrc_archive.nrc_recipe_id)))
  ORDER BY nrc_recipe_param.create_time, nrc_archive.nrc_recipe_id;


--
-- Name: VIEW nrc_archive_basic; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON VIEW nutrition.nrc_archive_basic IS 'basic view of archive data';


--
-- Name: nrc_recipe_animal_input; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.nrc_recipe_animal_input (
    nrc_recipe integer NOT NULL,
    "AnimalType" text DEFAULT 'Lactating Cow'::text NOT NULL,
    "Age" numeric DEFAULT 65 NOT NULL,
    "BW" numeric DEFAULT 680 NOT NULL,
    "DaysPreg" numeric DEFAULT 0 NOT NULL,
    "CS" numeric DEFAULT 3.0 NOT NULL,
    "DaysInMilk" numeric DEFAULT 90 NOT NULL,
    "LactNum" integer DEFAULT 3 NOT NULL,
    "FirstCalf" numeric DEFAULT 24 NOT NULL,
    "CalfInt" numeric DEFAULT 12 NOT NULL,
    "DesiredADG" numeric DEFAULT 0 NOT NULL,
    "UseTargetADG" boolean DEFAULT true NOT NULL,
    "CalfBW" numeric DEFAULT 0 NOT NULL,
    "CalfTemp" numeric DEFAULT 0 NOT NULL,
    "MW" numeric DEFAULT 680 NOT NULL,
    "MWFromBreed" boolean DEFAULT true NOT NULL,
    "Breed" text DEFAULT 'Holstein'::text NOT NULL,
    "CBW" numeric DEFAULT 35 NOT NULL,
    "CBWFromMW" boolean DEFAULT false NOT NULL,
    "MilkProd" numeric DEFAULT 54.5 NOT NULL,
    "MilkFat" numeric DEFAULT 3.5 NOT NULL,
    "ShowMilkTrue" boolean DEFAULT false NOT NULL,
    "MilkTrueProtein" numeric DEFAULT 3.0 NOT NULL,
    "Lactose" numeric DEFAULT 4.8 NOT NULL,
    "Temp" numeric DEFAULT 20 NOT NULL,
    "PrevTemp" numeric DEFAULT 0 NOT NULL,
    "WindSpeed" numeric DEFAULT 0 NOT NULL,
    "Grazing" numeric DEFAULT 0 NOT NULL,
    "Distance" numeric DEFAULT 0 NOT NULL,
    "Topography" text DEFAULT 'Flat'::text NOT NULL,
    "Trips" integer DEFAULT 0 NOT NULL,
    "CoatCond" text DEFAULT 'Clean/Dry'::text NOT NULL,
    "HeatStress" text DEFAULT 'None'::text NOT NULL,
    "HairDepth" numeric DEFAULT 0 NOT NULL,
    "NightCooling" boolean DEFAULT false NOT NULL
);


--
-- Name: TABLE nrc_recipe_animal_input; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.nrc_recipe_animal_input IS 'model input paramaters';


--
-- Name: nrc_recipe_item; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.nrc_recipe_item (
    nrc_recipe integer DEFAULT 1 NOT NULL,
    feed_library_name text NOT NULL,
    kg_day_dry numeric DEFAULT 0 NOT NULL,
    feedcurr_id text,
    id integer DEFAULT nextval('nutrition.nutrition_sequence'::regclass) NOT NULL
);


--
-- Name: COLUMN nrc_recipe_item.feedcurr_id; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.nrc_recipe_item.feedcurr_id IS 'can be null, but not if active.';


--
-- Name: nrc_recipe_location; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.nrc_recipe_location (
    nrc_recipe integer NOT NULL,
    location_id integer NOT NULL,
    modifier numeric DEFAULT 1 NOT NULL
);


--
-- Name: TABLE nrc_recipe_location; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.nrc_recipe_location IS 'holds information on location for mix and how much.';


--
-- Name: nrc_recipe_active_staging; Type: VIEW; Schema: nutrition; Owner: -
--

CREATE VIEW nutrition.nrc_recipe_active_staging AS
 SELECT nrc_recipe_param.nrc_recipe,
    nrc_recipe_param.title,
    nrc_recipe_param.comment,
    nrc_recipe_param.status,
    nrc_recipe_param.create_time,
    nrc_recipe_param.update_time,
    ( SELECT array_agg(c.location_id) AS array_agg
           FROM nutrition.nrc_recipe_location c
          WHERE (c.nrc_recipe = nrc_recipe_param.nrc_recipe)) AS locations,
    nrc_recipe_animal_input."AnimalType",
    nrc_recipe_animal_input."Age",
    nrc_recipe_animal_input."BW",
    nrc_recipe_animal_input."DaysPreg",
    nrc_recipe_animal_input."CS",
    nrc_recipe_animal_input."DaysInMilk",
    nrc_recipe_animal_input."LactNum",
    nrc_recipe_animal_input."MilkProd",
    nrc_recipe_animal_input."MilkFat",
    nrc_recipe_animal_input."MilkTrueProtein",
    nrc_recipe_item.feed_library_name,
    nrc_recipe_item.kg_day_dry,
    nrc_recipe_item.feedcurr_id,
    sum(nrc_recipe_item.kg_day_dry) OVER (PARTITION BY nrc_recipe_param.nrc_recipe) AS kg_day_dry_sum
   FROM ((nutrition.nrc_recipe_param
     LEFT JOIN nutrition.nrc_recipe_animal_input ON ((nrc_recipe_param.nrc_recipe = nrc_recipe_animal_input.nrc_recipe)))
     LEFT JOIN nutrition.nrc_recipe_item ON ((nrc_recipe_item.nrc_recipe = nrc_recipe_param.nrc_recipe)))
  WHERE (((nrc_recipe_param.status)::text = 'active'::text) OR ((nrc_recipe_param.status)::text = 'staging'::text));


--
-- Name: VIEW nrc_recipe_active_staging; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON VIEW nutrition.nrc_recipe_active_staging IS 'active or staging';


--
-- Name: nrc_recipe_animal_input_nrc_recipe_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.nrc_recipe_animal_input_nrc_recipe_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: nrc_recipe_animal_input_nrc_recipe_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.nrc_recipe_animal_input_nrc_recipe_seq OWNED BY nutrition.nrc_recipe_animal_input.nrc_recipe;


--
-- Name: nrc_recipe_item_feed_log; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.nrc_recipe_item_feed_log (
    start_mass numeric,
    end_mass numeric,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    nrc_recipe_item_id integer NOT NULL
);


--
-- Name: TABLE nrc_recipe_item_feed_log; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.nrc_recipe_item_feed_log IS 'records what was actually fed for each ingredient when employee feeds.';


--
-- Name: COLUMN nrc_recipe_item_feed_log.nrc_recipe_item_id; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.nrc_recipe_item_feed_log.nrc_recipe_item_id IS 'link to nrc_recipe_item';


--
-- Name: recipe_item; Type: TABLE; Schema: nutrition; Owner: -
--

CREATE TABLE nutrition.recipe_item (
    id integer NOT NULL,
    feedcurr_id character varying NOT NULL,
    kg_day_dry numeric NOT NULL,
    create_time timestamp without time zone NOT NULL,
    update_time timestamp without time zone NOT NULL,
    userid character(32) NOT NULL,
    recipe_id integer,
    round_to_tens boolean DEFAULT true NOT NULL,
    fed_free_choice boolean DEFAULT false NOT NULL,
    rounding_accuracy numeric DEFAULT 1 NOT NULL
);


--
-- Name: TABLE recipe_item; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON TABLE nutrition.recipe_item IS 'DEPRECATED. holds individual parts of the recipe.';


--
-- Name: COLUMN recipe_item.round_to_tens; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.recipe_item.round_to_tens IS 'tells whether to round to 10''s place or not.';


--
-- Name: COLUMN recipe_item.fed_free_choice; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.recipe_item.fed_free_choice IS 'when true item is not included in recipe totals.';


--
-- Name: COLUMN recipe_item.rounding_accuracy; Type: COMMENT; Schema: nutrition; Owner: -
--

COMMENT ON COLUMN nutrition.recipe_item.rounding_accuracy IS 'place according to php -3 to 4';


--
-- Name: recipe_item_id_seq; Type: SEQUENCE; Schema: nutrition; Owner: -
--

CREATE SEQUENCE nutrition.recipe_item_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: recipe_item_id_seq; Type: SEQUENCE OWNED BY; Schema: nutrition; Owner: -
--

ALTER SEQUENCE nutrition.recipe_item_id_seq OWNED BY nutrition.recipe_item.id;


--
-- Name: bovine; Type: TABLE; Schema: picture; Owner: -
--

CREATE TABLE picture.bovine (
    id integer NOT NULL,
    bovine_id integer,
    event_time timestamp without time zone,
    picture bytea,
    userid character varying(32) NOT NULL
);


--
-- Name: TABLE bovine; Type: COMMENT; Schema: picture; Owner: -
--

COMMENT ON TABLE picture.bovine IS 'stores pictures of animals';


--
-- Name: picture_id_seq; Type: SEQUENCE; Schema: picture; Owner: -
--

CREATE SEQUENCE picture.picture_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: picture_id_seq; Type: SEQUENCE OWNED BY; Schema: picture; Owner: -
--

ALTER SEQUENCE picture.picture_id_seq OWNED BY picture.bovine.id;


--
-- Name: machine; Type: TABLE; Schema: picture; Owner: -
--

CREATE TABLE picture.machine (
    id integer DEFAULT nextval('picture.picture_id_seq'::regclass) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    picture bytea NOT NULL,
    machine_id integer NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: machine_comment; Type: TABLE; Schema: picture; Owner: -
--

CREATE TABLE picture.machine_comment (
    id integer DEFAULT nextval('picture.picture_id_seq'::regclass) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    picture bytea NOT NULL,
    machine_id integer NOT NULL,
    userid character(32) NOT NULL
);


--
-- Name: picture_combined; Type: VIEW; Schema: picture; Owner: -
--

CREATE VIEW picture.picture_combined AS
 SELECT bovine.id,
    bovine.event_time,
    bovine.picture,
    bovine.userid
   FROM picture.bovine
UNION
 SELECT machine.id,
    machine.event_time,
    machine.picture,
    machine.userid
   FROM picture.machine
UNION
 SELECT machine_comment.id,
    machine_comment.event_time,
    machine_comment.picture,
    machine_comment.userid
   FROM picture.machine_comment;


--
-- Name: roadstable_gid_seq; Type: SEQUENCE; Schema: picture; Owner: -
--

CREATE SEQUENCE picture.roadstable_gid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: classificationreport_temp; Type: TABLE; Schema: system; Owner: -
--

CREATE TABLE system.classificationreport_temp (
    bovine_id integer NOT NULL,
    checked boolean NOT NULL,
    completed boolean DEFAULT false NOT NULL
);


--
-- Name: TABLE classificationreport_temp; Type: COMMENT; Schema: system; Owner: -
--

COMMENT ON TABLE system.classificationreport_temp IS 'holds list of bovine id''s to create report with';


--
-- Name: COLUMN classificationreport_temp.checked; Type: COMMENT; Schema: system; Owner: -
--

COMMENT ON COLUMN system.classificationreport_temp.checked IS 'true when checked';


--
-- Name: login_log; Type: TABLE; Schema: system; Owner: -
--

CREATE TABLE system.login_log (
    uuid uuid NOT NULL,
    userid character(32) NOT NULL,
    event_time timestamp without time zone NOT NULL,
    user_agent character varying NOT NULL,
    ip inet NOT NULL
);


--
-- Name: TABLE login_log; Type: COMMENT; Schema: system; Owner: -
--

COMMENT ON TABLE system.login_log IS 'logs everyone who logs in';


--
-- Name: COLUMN login_log.ip; Type: COMMENT; Schema: system; Owner: -
--

COMMENT ON COLUMN system.login_log.ip IS 'remote ip address';


--
-- Name: pg_table_nonindex_x; Type: VIEW; Schema: system; Owner: -
--

CREATE VIEW system.pg_table_nonindex_x AS
 SELECT x1.table_in_trouble,
    pg_relation_size((x1.table_in_trouble)::regclass) AS sz_n_byts,
    x1.seq_scan,
    x1.idx_scan,
        CASE
            WHEN (pg_relation_size((x1.table_in_trouble)::regclass) > 500000000) THEN 'Exceeds 500 megs, too large to count in a view. For a count, count individually'::text
            ELSE (count(x1.table_in_trouble))::text
        END AS tbl_rec_count,
    x1.priority
   FROM ( SELECT (((pg_stat_all_tables.schemaname)::text || '.'::text) || (pg_stat_all_tables.relname)::text) AS table_in_trouble,
            pg_stat_all_tables.seq_scan,
            pg_stat_all_tables.idx_scan,
                CASE
                    WHEN ((pg_stat_all_tables.seq_scan - pg_stat_all_tables.idx_scan) < 500) THEN 'Minor Problem'::text
                    WHEN (((pg_stat_all_tables.seq_scan - pg_stat_all_tables.idx_scan) >= 500) AND ((pg_stat_all_tables.seq_scan - pg_stat_all_tables.idx_scan) < 2500)) THEN 'Major Problem'::text
                    WHEN ((pg_stat_all_tables.seq_scan - pg_stat_all_tables.idx_scan) >= 2500) THEN 'Extreme Problem'::text
                    ELSE NULL::text
                END AS priority
           FROM pg_stat_all_tables
          WHERE ((pg_stat_all_tables.seq_scan > pg_stat_all_tables.idx_scan) AND (pg_stat_all_tables.schemaname <> 'pg_catalog'::name) AND (pg_stat_all_tables.seq_scan > 100))) x1
  GROUP BY x1.table_in_trouble, x1.seq_scan, x1.idx_scan, x1.priority
  ORDER BY x1.priority DESC, x1.seq_scan;


--
-- Name: VIEW pg_table_nonindex_x; Type: COMMENT; Schema: system; Owner: -
--

COMMENT ON VIEW system.pg_table_nonindex_x IS 'Shows index probelms with tables in DB';


--
-- Name: salesreport_temp; Type: TABLE; Schema: system; Owner: -
--

CREATE TABLE system.salesreport_temp (
    bovine_id integer NOT NULL,
    checked boolean NOT NULL
);


--
-- Name: search_view; Type: VIEW; Schema: system; Owner: -
--

CREATE VIEW system.search_view AS
 WITH temp AS (
         SELECT bovine.id,
            (bovine.local_number)::text AS textid,
            'bovineid'::text AS type
           FROM bovinemanagement.bovine
        UNION
         SELECT calf_potential_name.bovine_id AS id,
            (calf_potential_name.potential_name)::text AS textid,
            'bovineid'::text AS type
           FROM bovinemanagement.calf_potential_name
        UNION
         SELECT bovinecurr.id,
            bovinemanagement.short_name((bovinecurr.full_name)::text) AS textid,
            'bovineid'::text AS type
           FROM bovinemanagement.bovinecurr
        UNION
         SELECT bovinecurr.id,
            bovinecurr.full_reg_number AS textid,
            'bovineid'::text AS type
           FROM bovinemanagement.bovinecurr
        UNION
         SELECT page.pageid AS id,
            (page.title)::text AS textid,
            'pageid'::text AS type
           FROM intwebsite.page
        UNION
         SELECT field.id,
            (field.alpha_numeric_id)::text AS textid,
            'fieldid'::text AS type
           FROM cropping.field
        UNION
         SELECT field.id,
            (field.common_name)::text AS textid,
            'fieldid'::text AS type
           FROM cropping.field
        UNION
         SELECT machine.id,
            machine.name AS textid,
            'machineid'::text AS type
           FROM machinery.machine
        )
 SELECT temp.id,
    temp.textid,
    temp.type
   FROM temp
  WHERE (temp.textid IS NOT NULL);


--
-- Name: VIEW search_view; Type: COMMENT; Schema: system; Owner: -
--

COMMENT ON VIEW system.search_view IS 'used as the search space for the query bar in website';


--
-- Name: update_log; Type: TABLE; Schema: system; Owner: -
--

CREATE TABLE system.update_log (
    schema_name text NOT NULL,
    table_name text NOT NULL,
    last_update timestamp without time zone DEFAULT now() NOT NULL
);


--
-- Name: TABLE update_log; Type: COMMENT; Schema: system; Owner: -
--

COMMENT ON TABLE system.update_log IS 'holds last access to a schema/table';


--
-- Name: identifikation; Type: FOREIGN TABLE; Schema: urban_feeder_foreign_tiere; Owner: -
--

CREATE FOREIGN TABLE urban_feeder_foreign_tiere.identifikation (
    tiere_id bigint NOT NULL,
    responder_nr text NOT NULL,
    einstalldatum date NOT NULL,
    kurvennr smallint NOT NULL,
    faelscher integer NOT NULL,
    aktiv boolean NOT NULL,
    standort smallint NOT NULL,
    con10_id text NOT NULL,
    tiri2_erkennung_nr smallint NOT NULL,
    tiri2_id text NOT NULL,
    debug_futtermenge real NOT NULL,
    debug_zeitpunkt timestamp without time zone NOT NULL,
    geburtsdatum date,
    tier_nr bigint NOT NULL,
    herdbuch_nr bigint,
    futterwerte_neu_berechnen boolean NOT NULL,
    laendercode smallint NOT NULL,
    elektronische_ersterkennung timestamp without time zone,
    tier_loeschen_flag boolean NOT NULL,
    responder_erneut_aus_tiernr_erkennen boolean NOT NULL
)
SERVER foreign_server_urban_feeder
OPTIONS (
    schema_name 'tiere',
    table_name 'identifikation'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN tiere_id OPTIONS (
    column_name 'tiere_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN responder_nr OPTIONS (
    column_name 'responder_nr'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN einstalldatum OPTIONS (
    column_name 'einstalldatum'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN kurvennr OPTIONS (
    column_name 'kurvennr'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN faelscher OPTIONS (
    column_name 'faelscher'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN aktiv OPTIONS (
    column_name 'aktiv'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN standort OPTIONS (
    column_name 'standort'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN con10_id OPTIONS (
    column_name 'con10_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN tiri2_erkennung_nr OPTIONS (
    column_name 'tiri2_erkennung_nr'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN tiri2_id OPTIONS (
    column_name 'tiri2_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN debug_futtermenge OPTIONS (
    column_name 'debug_futtermenge'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN debug_zeitpunkt OPTIONS (
    column_name 'debug_zeitpunkt'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN geburtsdatum OPTIONS (
    column_name 'geburtsdatum'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN tier_nr OPTIONS (
    column_name 'tier_nr'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN herdbuch_nr OPTIONS (
    column_name 'herdbuch_nr'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN futterwerte_neu_berechnen OPTIONS (
    column_name 'futterwerte_neu_berechnen'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN laendercode OPTIONS (
    column_name 'laendercode'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN elektronische_ersterkennung OPTIONS (
    column_name 'elektronische_ersterkennung'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN tier_loeschen_flag OPTIONS (
    column_name 'tier_loeschen_flag'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.identifikation ALTER COLUMN responder_erneut_aus_tiernr_erkennen OPTIONS (
    column_name 'responder_erneut_aus_tiernr_erkennen'
);


--
-- Name: tabelle_anrecht_seit; Type: FOREIGN TABLE; Schema: urban_feeder_foreign_tiere; Owner: -
--

CREATE FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_anrecht_seit (
    anrecht_seit_id bigint NOT NULL,
    stationsbesuch_id bigint NOT NULL,
    tiere_id bigint NOT NULL,
    anrecht_seit timestamp without time zone NOT NULL,
    anrecht_seit_menge double precision NOT NULL
)
SERVER foreign_server_urban_feeder
OPTIONS (
    schema_name 'tiere',
    table_name 'tabelle_anrecht_seit'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_anrecht_seit ALTER COLUMN anrecht_seit_id OPTIONS (
    column_name 'anrecht_seit_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_anrecht_seit ALTER COLUMN stationsbesuch_id OPTIONS (
    column_name 'stationsbesuch_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_anrecht_seit ALTER COLUMN tiere_id OPTIONS (
    column_name 'tiere_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_anrecht_seit ALTER COLUMN anrecht_seit OPTIONS (
    column_name 'anrecht_seit'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_anrecht_seit ALTER COLUMN anrecht_seit_menge OPTIONS (
    column_name 'anrecht_seit_menge'
);


--
-- Name: tabelle_stationsbesuch; Type: FOREIGN TABLE; Schema: urban_feeder_foreign_tiere; Owner: -
--

CREATE FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch (
    stationsbesuch_id bigint NOT NULL,
    tiere_id bigint NOT NULL,
    erste_erkennung timestamp without time zone NOT NULL,
    letzte_erkennung timestamp without time zone NOT NULL,
    noch_in_der_station boolean NOT NULL,
    gebaeude integer NOT NULL,
    terminal integer NOT NULL,
    automat integer NOT NULL,
    station integer NOT NULL,
    box integer NOT NULL,
    aktuelle_funktion integer NOT NULL
)
SERVER foreign_server_urban_feeder
OPTIONS (
    schema_name 'tiere',
    table_name 'tabelle_stationsbesuch'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN stationsbesuch_id OPTIONS (
    column_name 'stationsbesuch_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN tiere_id OPTIONS (
    column_name 'tiere_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN erste_erkennung OPTIONS (
    column_name 'erste_erkennung'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN letzte_erkennung OPTIONS (
    column_name 'letzte_erkennung'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN noch_in_der_station OPTIONS (
    column_name 'noch_in_der_station'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN gebaeude OPTIONS (
    column_name 'gebaeude'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN terminal OPTIONS (
    column_name 'terminal'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN automat OPTIONS (
    column_name 'automat'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN station OPTIONS (
    column_name 'station'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN box OPTIONS (
    column_name 'box'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_stationsbesuch ALTER COLUMN aktuelle_funktion OPTIONS (
    column_name 'aktuelle_funktion'
);


--
-- Name: tabelle_trinkgeschwindigkeit; Type: FOREIGN TABLE; Schema: urban_feeder_foreign_tiere; Owner: -
--

CREATE FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_trinkgeschwindigkeit (
    trinkgeschwindigkeit_id bigint NOT NULL,
    stationsbesuch_id bigint NOT NULL,
    tiere_id bigint NOT NULL,
    trinkmenge double precision NOT NULL,
    trinkzeit_sekunden double precision NOT NULL,
    zeitstempel timestamp without time zone NOT NULL,
    ventil_zugeschaltet boolean NOT NULL,
    ventil_abgeschaltet boolean NOT NULL
)
SERVER foreign_server_urban_feeder
OPTIONS (
    schema_name 'tiere',
    table_name 'tabelle_trinkgeschwindigkeit'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_trinkgeschwindigkeit ALTER COLUMN trinkgeschwindigkeit_id OPTIONS (
    column_name 'trinkgeschwindigkeit_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_trinkgeschwindigkeit ALTER COLUMN stationsbesuch_id OPTIONS (
    column_name 'stationsbesuch_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_trinkgeschwindigkeit ALTER COLUMN tiere_id OPTIONS (
    column_name 'tiere_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_trinkgeschwindigkeit ALTER COLUMN trinkmenge OPTIONS (
    column_name 'trinkmenge'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_trinkgeschwindigkeit ALTER COLUMN trinkzeit_sekunden OPTIONS (
    column_name 'trinkzeit_sekunden'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_trinkgeschwindigkeit ALTER COLUMN zeitstempel OPTIONS (
    column_name 'zeitstempel'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_trinkgeschwindigkeit ALTER COLUMN ventil_zugeschaltet OPTIONS (
    column_name 'ventil_zugeschaltet'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tabelle_trinkgeschwindigkeit ALTER COLUMN ventil_abgeschaltet OPTIONS (
    column_name 'ventil_abgeschaltet'
);


--
-- Name: tageswerte_verbrauch_milch; Type: FOREIGN TABLE; Schema: urban_feeder_foreign_tiere; Owner: -
--

CREATE FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch (
    tiere_id bigint NOT NULL,
    tag date NOT NULL,
    tagesmenge_milch double precision NOT NULL,
    tagesverbrauch_milch double precision NOT NULL,
    tagesverbrauch_mat1 double precision NOT NULL,
    tagesverbrauch_mat2 double precision NOT NULL,
    tagesverbrauch_wasser double precision NOT NULL,
    tagesverbrauch_vollmilch double precision NOT NULL,
    letztes_update timestamp without time zone NOT NULL,
    tagesmenge_futteralarm double precision NOT NULL,
    tagesmenge_note double precision NOT NULL,
    tagesmenge_note_farbe smallint NOT NULL,
    tagesgesundheit_note double precision NOT NULL,
    tagesgesundheit_note_farbe smallint NOT NULL,
    gesundheit_schlechtester_note double precision NOT NULL,
    gesundheit_schlechtester_note_farbe smallint NOT NULL,
    tagesverbrauch_adlib_milch double precision NOT NULL,
    zusatz_id bigint[] NOT NULL,
    tagesmenge_zusatz double precision[] NOT NULL,
    tagesverbauch_zusatz double precision[] NOT NULL
)
SERVER foreign_server_urban_feeder
OPTIONS (
    schema_name 'tiere',
    table_name 'tageswerte_verbrauch_milch'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tiere_id OPTIONS (
    column_name 'tiere_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tag OPTIONS (
    column_name 'tag'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesmenge_milch OPTIONS (
    column_name 'tagesmenge_milch'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesverbrauch_milch OPTIONS (
    column_name 'tagesverbrauch_milch'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesverbrauch_mat1 OPTIONS (
    column_name 'tagesverbrauch_mat1'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesverbrauch_mat2 OPTIONS (
    column_name 'tagesverbrauch_mat2'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesverbrauch_wasser OPTIONS (
    column_name 'tagesverbrauch_wasser'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesverbrauch_vollmilch OPTIONS (
    column_name 'tagesverbrauch_vollmilch'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN letztes_update OPTIONS (
    column_name 'letztes_update'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesmenge_futteralarm OPTIONS (
    column_name 'tagesmenge_futteralarm'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesmenge_note OPTIONS (
    column_name 'tagesmenge_note'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesmenge_note_farbe OPTIONS (
    column_name 'tagesmenge_note_farbe'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesgesundheit_note OPTIONS (
    column_name 'tagesgesundheit_note'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesgesundheit_note_farbe OPTIONS (
    column_name 'tagesgesundheit_note_farbe'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN gesundheit_schlechtester_note OPTIONS (
    column_name 'gesundheit_schlechtester_note'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN gesundheit_schlechtester_note_farbe OPTIONS (
    column_name 'gesundheit_schlechtester_note_farbe'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesverbrauch_adlib_milch OPTIONS (
    column_name 'tagesverbrauch_adlib_milch'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN zusatz_id OPTIONS (
    column_name 'zusatz_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesmenge_zusatz OPTIONS (
    column_name 'tagesmenge_zusatz'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.tageswerte_verbrauch_milch ALTER COLUMN tagesverbauch_zusatz OPTIONS (
    column_name 'tagesverbauch_zusatz'
);


--
-- Name: view_mh_aktuelle_tier_bewertungen; Type: FOREIGN TABLE; Schema: urban_feeder_foreign_tiere; Owner: -
--

CREATE FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen (
    tiere_id bigint,
    responder_nr text,
    bewertung_futter_24h double precision,
    farbe_bewertung_futter_24h integer,
    bewertung_gesundheit_72h double precision,
    farbe_bewertung_gesundheit_72h integer,
    letzte_berechnung_um timestamp without time zone,
    zustaendiges_terminal integer,
    zustaendiger_automat integer,
    gefressen_letzte_24h double precision
)
SERVER foreign_server_urban_feeder
OPTIONS (
    schema_name 'tiere',
    table_name 'view_mh_aktuelle_tier_bewertungen'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN tiere_id OPTIONS (
    column_name 'tiere_id'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN responder_nr OPTIONS (
    column_name 'responder_nr'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN bewertung_futter_24h OPTIONS (
    column_name 'bewertung_futter_24h'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN farbe_bewertung_futter_24h OPTIONS (
    column_name 'farbe_bewertung_futter_24h'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN bewertung_gesundheit_72h OPTIONS (
    column_name 'bewertung_gesundheit_72h'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN farbe_bewertung_gesundheit_72h OPTIONS (
    column_name 'farbe_bewertung_gesundheit_72h'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN letzte_berechnung_um OPTIONS (
    column_name 'letzte_berechnung_um'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN zustaendiges_terminal OPTIONS (
    column_name 'zustaendiges_terminal'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN zustaendiger_automat OPTIONS (
    column_name 'zustaendiger_automat'
);
ALTER FOREIGN TABLE urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ALTER COLUMN gefressen_letzte_24h OPTIONS (
    column_name 'gefressen_letzte_24h'
);


--
-- Name: groups; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.groups (
    id integer NOT NULL,
    name character(32) NOT NULL
);


--
-- Name: groups_id_seq; Type: SEQUENCE; Schema: wcauthentication; Owner: -
--

CREATE SEQUENCE wcauthentication.groups_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: groups_id_seq; Type: SEQUENCE OWNED BY; Schema: wcauthentication; Owner: -
--

ALTER SEQUENCE wcauthentication.groups_id_seq OWNED BY wcauthentication.groups.id;


--
-- Name: oauth_access_tokens; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.oauth_access_tokens (
    access_token character varying(40) NOT NULL,
    client_id character varying(80) NOT NULL,
    user_id character varying(255),
    expires timestamp without time zone NOT NULL,
    scope character varying(2000)
);


--
-- Name: oauth_authorization_codes; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.oauth_authorization_codes (
    authorization_code character varying(40) NOT NULL,
    client_id character varying(80) NOT NULL,
    user_id character varying(255),
    redirect_uri character varying(2000),
    expires timestamp without time zone NOT NULL,
    scope character varying(2000)
);


--
-- Name: oauth_clients; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.oauth_clients (
    client_id character varying(80) NOT NULL,
    client_secret character varying(80) NOT NULL,
    redirect_uri character varying(2000) NOT NULL,
    grant_types character varying(80),
    scope character varying(100),
    user_id character varying(80)
);


--
-- Name: COLUMN oauth_clients.grant_types; Type: COMMENT; Schema: wcauthentication; Owner: -
--

COMMENT ON COLUMN wcauthentication.oauth_clients.grant_types IS 'must be null';


--
-- Name: oauth_jwt; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.oauth_jwt (
    client_id character varying(80) NOT NULL,
    subject character varying(80),
    public_key character varying(2000)
);


--
-- Name: oauth_refresh_tokens; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.oauth_refresh_tokens (
    refresh_token character varying(40) NOT NULL,
    client_id character varying(80) NOT NULL,
    user_id character varying(255),
    expires timestamp without time zone NOT NULL,
    scope character varying(2000)
);


--
-- Name: oauth_scopes; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.oauth_scopes (
    scope text NOT NULL,
    is_default boolean NOT NULL
);


--
-- Name: oauth_users; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.oauth_users (
    username character varying(255) NOT NULL,
    password character varying(2000),
    first_name character varying(255) DEFAULT NULL::character varying,
    last_name character varying(255) DEFAULT NULL::character varying,
    email character varying(2000) DEFAULT NULL::character varying,
    "email_verified " integer,
    scope character varying(2000) DEFAULT NULL::character varying
);


--
-- Name: owners; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.owners (
    ownerid character varying NOT NULL
);


--
-- Name: TABLE owners; Type: COMMENT; Schema: wcauthentication; Owner: -
--

COMMENT ON TABLE wcauthentication.owners IS 'list of different owners for bovines.';


--
-- Name: COLUMN owners.ownerid; Type: COMMENT; Schema: wcauthentication; Owner: -
--

COMMENT ON COLUMN wcauthentication.owners.ownerid IS 'owner name';


--
-- Name: users; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.users (
    userid character varying(32) NOT NULL,
    firstname character(32),
    lastname character(32),
    email character(256),
    active boolean DEFAULT false NOT NULL,
    onfarm boolean DEFAULT true NOT NULL,
    testing boolean DEFAULT false NOT NULL,
    openid character varying
);


--
-- Name: COLUMN users.active; Type: COMMENT; Schema: wcauthentication; Owner: -
--

COMMENT ON COLUMN wcauthentication.users.active IS 'null when user deleted.';


--
-- Name: COLUMN users.onfarm; Type: COMMENT; Schema: wcauthentication; Owner: -
--

COMMENT ON COLUMN wcauthentication.users.onfarm IS 'user is employed by the farm';


--
-- Name: COLUMN users.testing; Type: COMMENT; Schema: wcauthentication; Owner: -
--

COMMENT ON COLUMN wcauthentication.users.testing IS 'shows which accounts are for testing';


--
-- Name: COLUMN users.openid; Type: COMMENT; Schema: wcauthentication; Owner: -
--

COMMENT ON COLUMN wcauthentication.users.openid IS 'stores google openid unique id';


--
-- Name: users_google; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.users_google (
    google_id numeric(21,0) NOT NULL,
    google_name character varying(128) NOT NULL,
    google_email character varying(128) NOT NULL,
    google_link character varying(2048) NOT NULL,
    google_picture_link character varying(2048) NOT NULL,
    google_refresh_token character varying(1024) NOT NULL
);


--
-- Name: TABLE users_google; Type: COMMENT; Schema: wcauthentication; Owner: -
--

COMMENT ON TABLE wcauthentication.users_google IS 'holds username, etc. when google person logs in. ';


--
-- Name: users_in_groups; Type: TABLE; Schema: wcauthentication; Owner: -
--

CREATE TABLE wcauthentication.users_in_groups (
    userid character varying(32) NOT NULL,
    groupid character varying(32) NOT NULL
);


--
-- Name: sync id; Type: DEFAULT; Schema: alpro; Owner: -
--

ALTER TABLE ONLY alpro.sync ALTER COLUMN id SET DEFAULT nextval('alpro.sync_id_seq'::regclass);


--
-- Name: beef_report id; Type: DEFAULT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.beef_report ALTER COLUMN id SET DEFAULT nextval('batch.beef_report_id_seq'::regclass);


--
-- Name: ccia_reported id; Type: DEFAULT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.ccia_reported ALTER COLUMN id SET DEFAULT nextval('batch.ccia_reported_id_seq'::regclass);


--
-- Name: daily_number_cows_milking id; Type: DEFAULT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.daily_number_cows_milking ALTER COLUMN id SET DEFAULT nextval('batch.daily_number_cows_milking_id_seq'::regclass);


--
-- Name: error_curr id; Type: DEFAULT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.error_curr ALTER COLUMN id SET DEFAULT nextval('batch.error_curr_id_seq'::regclass);


--
-- Name: milk_pickup_event id; Type: DEFAULT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.milk_pickup_event ALTER COLUMN id SET DEFAULT nextval('batch.milk_pickup_event_id_seq'::regclass);


--
-- Name: nb_bulk_tank_sample id; Type: DEFAULT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.nb_bulk_tank_sample ALTER COLUMN id SET DEFAULT nextval('batch.provincial_milk_test_data_id_seq'::regclass);


--
-- Name: quota id; Type: DEFAULT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.quota ALTER COLUMN id SET DEFAULT nextval('batch.quota_id_seq'::regclass);


--
-- Name: bovine id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.bovine ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.bovine_idx_seq'::regclass);


--
-- Name: breeding_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.breeding_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.breeding_event_id_seq'::regclass);


--
-- Name: calf_milk id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calf_milk ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.calf_milk_id_seq'::regclass);


--
-- Name: calf_potential_name id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calf_potential_name ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.calf_potential_name_id_seq'::regclass);


--
-- Name: calving_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.calving_event_id_seq'::regclass);


--
-- Name: calving_event_ease_types id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event_ease_types ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.calving_stats_ease_types_id_seq'::regclass);


--
-- Name: comment_custom id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.comment_custom ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.comment_reproductive_id_seq'::regclass);


--
-- Name: cull_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.cull_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.cull_event_id_seq'::regclass);


--
-- Name: cull_list_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.cull_list_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.cull_list_event_id_seq'::regclass);


--
-- Name: eartag id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.eartag ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.eartag_id_seq'::regclass);


--
-- Name: embryo_flush id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_flush ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.embryo_flush_id_seq'::regclass);


--
-- Name: embryo_implant id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.embryo_recipient_id_seq'::regclass);


--
-- Name: embryo_implant_comment id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant_comment ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.embryo_implant_comment_id_seq'::regclass);


--
-- Name: embryo_straw id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_straw ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.embryo_straw_id_seq'::regclass);


--
-- Name: estrus_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.estrus_event_id_seq'::regclass);


--
-- Name: estrus_type id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_type ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.estrus_type_id_seq'::regclass);


--
-- Name: foot_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.foot_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.foot_diagnosis_event_id_seq'::regclass);


--
-- Name: foot_type id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.foot_type ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.foot_type_id_seq'::regclass);


--
-- Name: kamar_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.kamar_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.kamar_event_id_seq'::regclass);


--
-- Name: lactation id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.lactation ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.lactation_id_seq'::regclass);


--
-- Name: location id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.location_id_seq'::regclass);


--
-- Name: location_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.location_event_id_seq'::regclass);


--
-- Name: location_sort id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_sort ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.location_sort_id_seq'::regclass);


--
-- Name: medical_case id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_case ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.medical_generic_id_seq'::regclass);


--
-- Name: medicine id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medicine ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.medicine_id_seq'::regclass);


--
-- Name: palpate_comment id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.palpate_comment ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.palpate_comment_id_seq'::regclass);


--
-- Name: preg_check_comment id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_comment ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.preg_check_comment_id_seq'::regclass);


--
-- Name: preg_check_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.preg_check_event_id_seq'::regclass);


--
-- Name: preg_check_type id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_type ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.preg_check_type_id_seq'::regclass);


--
-- Name: production_item id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.production_item ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.production_item_id_seq'::regclass);


--
-- Name: protocol_event id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.protocol_event ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.breeding_protocol_event_id_seq1'::regclass);


--
-- Name: protocol_type id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.protocol_type ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.breeding_protocol_id_seq'::regclass);


--
-- Name: sale_price id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sale_price ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.sale_price_id_seq'::regclass);


--
-- Name: sale_price_comment id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sale_price_comment ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.sale_price_comment_id_seq'::regclass);


--
-- Name: semen_straw id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.semen_straw ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.semen_straw_id_seq'::regclass);


--
-- Name: service_picks id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.service_picks ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.service_picks_id_seq'::regclass);


--
-- Name: sire id; Type: DEFAULT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sire ALTER COLUMN id SET DEFAULT nextval('bovinemanagement.sire_id_seq'::regclass);


--
-- Name: datum id; Type: DEFAULT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.datum ALTER COLUMN id SET DEFAULT nextval('cropping.cropping_id_seq'::regclass);


--
-- Name: document id; Type: DEFAULT; Schema: documents; Owner: -
--

ALTER TABLE ONLY documents.document ALTER COLUMN id SET DEFAULT nextval('documents.document_id_seq'::regclass);


--
-- Name: coverage gid; Type: DEFAULT; Schema: gis; Owner: -
--

ALTER TABLE ONLY gis.coverage ALTER COLUMN gid SET DEFAULT nextval('gis.coverage_gid_seq'::regclass);


--
-- Name: roadstable gid; Type: DEFAULT; Schema: gis; Owner: -
--

ALTER TABLE ONLY gis.roadstable ALTER COLUMN gid SET DEFAULT nextval('gis.roadstable_gid_seq'::regclass);


--
-- Name: summary id; Type: DEFAULT; Schema: gis; Owner: -
--

ALTER TABLE ONLY gis.summary ALTER COLUMN id SET DEFAULT nextval('gis.summary_id_seq'::regclass);


--
-- Name: swaths gid; Type: DEFAULT; Schema: gis; Owner: -
--

ALTER TABLE ONLY gis.swaths ALTER COLUMN gid SET DEFAULT nextval('gis.swaths_gid_seq'::regclass);


--
-- Name: overtime id; Type: DEFAULT; Schema: hr; Owner: -
--

ALTER TABLE ONLY hr.overtime ALTER COLUMN id SET DEFAULT nextval('hr.overtime_id_seq'::regclass);


--
-- Name: time_sheet id; Type: DEFAULT; Schema: hr; Owner: -
--

ALTER TABLE ONLY hr.time_sheet ALTER COLUMN id SET DEFAULT nextval('hr.time_bank_id_seq'::regclass);


--
-- Name: ajax id; Type: DEFAULT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.ajax ALTER COLUMN id SET DEFAULT nextval('intwebsite.ajax_id_seq'::regclass);


--
-- Name: ajax_security id; Type: DEFAULT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.ajax_security ALTER COLUMN id SET DEFAULT nextval('intwebsite.ajax_security_id_seq'::regclass);


--
-- Name: page pageid; Type: DEFAULT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.page ALTER COLUMN pageid SET DEFAULT nextval('intwebsite.page_pageid_seq'::regclass);


--
-- Name: page_security id; Type: DEFAULT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.page_security ALTER COLUMN id SET DEFAULT nextval('intwebsite.page_security_id_seq'::regclass);


--
-- Name: comment id; Type: DEFAULT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.comment ALTER COLUMN id SET DEFAULT nextval('machinery.comment_id_seq'::regclass);


--
-- Name: hours_log id; Type: DEFAULT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.hours_log ALTER COLUMN id SET DEFAULT nextval('machinery.hours_log_id_seq'::regclass);


--
-- Name: machine id; Type: DEFAULT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.machine ALTER COLUMN id SET DEFAULT nextval('machinery.machine_id_seq'::regclass);


--
-- Name: service_administered id; Type: DEFAULT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.service_administered ALTER COLUMN id SET DEFAULT nextval('machinery.service_administered_id_seq'::regclass);


--
-- Name: service_item id; Type: DEFAULT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.service_item ALTER COLUMN id SET DEFAULT nextval('machinery.service_item_id_seq'::regclass);


--
-- Name: cqm_record17 id; Type: DEFAULT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.cqm_record17 ALTER COLUMN id SET DEFAULT nextval('misc.cqm_record17_id_seq'::regclass);


--
-- Name: purchases id; Type: DEFAULT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.purchases ALTER COLUMN id SET DEFAULT nextval('misc.purchases_id_seq'::regclass);


--
-- Name: sop id; Type: DEFAULT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.sop ALTER COLUMN id SET DEFAULT nextval('misc.sop_id_seq'::regclass);


--
-- Name: task id; Type: DEFAULT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.task ALTER COLUMN id SET DEFAULT nextval('misc.task_id_seq'::regclass);


--
-- Name: analysis_link id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.analysis_link ALTER COLUMN id SET DEFAULT nextval('nutrition.analysis_link_id_seq'::regclass);


--
-- Name: bag id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag ALTER COLUMN id SET DEFAULT nextval('nutrition.bag_ensiled_id_seq'::regclass);


--
-- Name: bag_comment id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_comment ALTER COLUMN id SET DEFAULT nextval('nutrition.bag_comment_id_seq'::regclass);


--
-- Name: bag_consumption id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_consumption ALTER COLUMN id SET DEFAULT nextval('nutrition.bag_consumption_id_seq'::regclass);


--
-- Name: bag_cost id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_cost ALTER COLUMN id SET DEFAULT nextval('nutrition.cost_id_seq'::regclass);


--
-- Name: bag_ensile_date id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_ensile_date ALTER COLUMN id SET DEFAULT nextval('nutrition.bag_ensile_date_id_seq'::regclass);


--
-- Name: bag_feed id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_feed ALTER COLUMN id SET DEFAULT nextval('nutrition.bag_feed_id_seq'::regclass);


--
-- Name: bag_field id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_field ALTER COLUMN id SET DEFAULT nextval('nutrition.bag_field_id_seq'::regclass);


--
-- Name: bag_moisture_test id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_moisture_test ALTER COLUMN id SET DEFAULT nextval('nutrition.bag_moisture_test_id_seq'::regclass);


--
-- Name: feed id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed ALTER COLUMN id SET DEFAULT nextval('nutrition.feed_id_seq1'::regclass);


--
-- Name: feed_location id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_location ALTER COLUMN id SET DEFAULT nextval('nutrition.feed_location_id_seq'::regclass);


--
-- Name: feed_type id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_type ALTER COLUMN id SET DEFAULT nextval('nutrition.feed_id_seq'::regclass);


--
-- Name: mix id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.mix ALTER COLUMN id SET DEFAULT nextval('nutrition.mix_id_seq'::regclass);


--
-- Name: recipe id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.recipe ALTER COLUMN id SET DEFAULT nextval('nutrition."3_id_seq"'::regclass);


--
-- Name: recipe_item id; Type: DEFAULT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.recipe_item ALTER COLUMN id SET DEFAULT nextval('nutrition.recipe_item_id_seq'::regclass);


--
-- Name: bovine id; Type: DEFAULT; Schema: picture; Owner: -
--

ALTER TABLE ONLY picture.bovine ALTER COLUMN id SET DEFAULT nextval('picture.picture_id_seq'::regclass);


--
-- Name: groups id; Type: DEFAULT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.groups ALTER COLUMN id SET DEFAULT nextval('wcauthentication.groups_id_seq'::regclass);


--
-- Name: cow cow_pkey; Type: CONSTRAINT; Schema: alpro; Owner: -
--

ALTER TABLE ONLY alpro.cow
    ADD CONSTRAINT cow_pkey PRIMARY KEY (bovine_id, date, milking_number);


--
-- Name: cron cron_pkey; Type: CONSTRAINT; Schema: alpro; Owner: -
--

ALTER TABLE ONLY alpro.cron
    ADD CONSTRAINT cron_pkey PRIMARY KEY (uuid);


--
-- Name: sort_gate_log sort_gate_log_bovine_id_event_time_key; Type: CONSTRAINT; Schema: alpro; Owner: -
--

ALTER TABLE ONLY alpro.sort_gate_log
    ADD CONSTRAINT sort_gate_log_bovine_id_event_time_key UNIQUE (bovine_id, event_time);


--
-- Name: sync sync_pkey; Type: CONSTRAINT; Schema: alpro; Owner: -
--

ALTER TABLE ONLY alpro.sync
    ADD CONSTRAINT sync_pkey PRIMARY KEY (id);


--
-- Name: ble_base ble_base_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_base
    ADD CONSTRAINT ble_base_pkey PRIMARY KEY (base_id);


--
-- Name: ble_other_tag ble_other_tag_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_other_tag
    ADD CONSTRAINT ble_other_tag_pkey PRIMARY KEY (tag_id, event_time);


--
-- Name: ble_tag_event_filtered ble_tag_event_filtered_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_tag_event_filtered
    ADD CONSTRAINT ble_tag_event_filtered_pkey PRIMARY KEY (tag_id, event_time);

ALTER TABLE bas.ble_tag_event_filtered CLUSTER ON ble_tag_event_filtered_pkey;


--
-- Name: ble_tag_event_filtered_variance_20min ble_tag_event_filtered_variance_20min_unique_tag_id_event_time; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_tag_event_filtered_variance_20min
    ADD CONSTRAINT ble_tag_event_filtered_variance_20min_unique_tag_id_event_time UNIQUE (tag_id, event_time);


--
-- Name: ble_tag_event_filtered_variance_7day ble_tag_event_filtered_variance_7day_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_tag_event_filtered_variance_7day
    ADD CONSTRAINT ble_tag_event_filtered_variance_7day_pkey PRIMARY KEY (tag_id, event_time);

ALTER TABLE bas.ble_tag_event_filtered_variance_7day CLUSTER ON ble_tag_event_filtered_variance_7day_pkey;


--
-- Name: ble_tag_event_filtered_variance_7day ble_tag_event_filtered_variance_7day_unique_tag_id_event_time; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_tag_event_filtered_variance_7day
    ADD CONSTRAINT ble_tag_event_filtered_variance_7day_unique_tag_id_event_time UNIQUE (tag_id, event_time);


--
-- Name: ble_tag_event_filtered_variance_final ble_tag_event_filtered_variance_final_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_tag_event_filtered_variance_final
    ADD CONSTRAINT ble_tag_event_filtered_variance_final_pkey PRIMARY KEY (tag_id, event_time);

ALTER TABLE bas.ble_tag_event_filtered_variance_final CLUSTER ON ble_tag_event_filtered_variance_final_pkey;


--
-- Name: ble_tag_event_filtered_variance_final ble_tag_event_filtered_variance_final_unique_tag_id_event_time; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_tag_event_filtered_variance_final
    ADD CONSTRAINT ble_tag_event_filtered_variance_final_unique_tag_id_event_time UNIQUE (tag_id, event_time);


--
-- Name: ble_tag_event_filtered_variance_20min ble_tag_event_filtered_variance_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_tag_event_filtered_variance_20min
    ADD CONSTRAINT ble_tag_event_filtered_variance_pkey PRIMARY KEY (tag_id, event_time);

ALTER TABLE bas.ble_tag_event_filtered_variance_20min CLUSTER ON ble_tag_event_filtered_variance_pkey;


--
-- Name: ble_tag_event ble_tag_event_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_tag_event
    ADD CONSTRAINT ble_tag_event_pkey PRIMARY KEY (event_time, base_id);


--
-- Name: calf_barn_scale_event_raw calf_barn_scale_event_raw_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.calf_barn_scale_event_raw
    ADD CONSTRAINT calf_barn_scale_event_raw_pkey PRIMARY KEY (event_time, scale_id);


--
-- Name: ble_tag_event_filtered_variance_10min dd_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.ble_tag_event_filtered_variance_10min
    ADD CONSTRAINT dd_pkey PRIMARY KEY (tag_id, event_time);


--
-- Name: feed_auger_event_range feed_auger_event_range_tank_id_event_range_key; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.feed_auger_event_range
    ADD CONSTRAINT feed_auger_event_range_tank_id_event_range_key UNIQUE (tank_id, event_range);


--
-- Name: feed_bin_weight_event feed_bin_weight_event_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.feed_bin_weight_event
    ADD CONSTRAINT feed_bin_weight_event_pkey PRIMARY KEY (tank_id, event_time);

ALTER TABLE bas.feed_bin_weight_event CLUSTER ON feed_bin_weight_event_pkey;


--
-- Name: tmr_event tmr_event_pkey; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.tmr_event
    ADD CONSTRAINT tmr_event_pkey PRIMARY KEY (event_time);


--
-- Name: vfd_event vfd_unique; Type: CONSTRAINT; Schema: bas; Owner: -
--

ALTER TABLE ONLY bas.vfd_event
    ADD CONSTRAINT vfd_unique UNIQUE (vfd_id, event_time, address_number);


--
-- Name: alpro_group_production alpro_group_production_event_time_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.alpro_group_production
    ADD CONSTRAINT alpro_group_production_event_time_key UNIQUE (event_time, location_id);


--
-- Name: credit batch.credit_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.credit
    ADD CONSTRAINT "batch.credit_pkey" PRIMARY KEY (id);


--
-- Name: beef_report beef_report_event_date_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.beef_report
    ADD CONSTRAINT beef_report_event_date_key UNIQUE (event_date);


--
-- Name: beef_report beef_report_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.beef_report
    ADD CONSTRAINT beef_report_pkey PRIMARY KEY (id);


--
-- Name: ccia_reported ccia_reported_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.ccia_reported
    ADD CONSTRAINT ccia_reported_pkey PRIMARY KEY (id);


--
-- Name: aggregate_site_data cdn_family_tree_data_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.aggregate_site_data
    ADD CONSTRAINT cdn_family_tree_data_pkey PRIMARY KEY (full_reg_number);


--
-- Name: cdn_progeny_potential_genetics combined_dam_sire_reg_number; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.cdn_progeny_potential_genetics
    ADD CONSTRAINT combined_dam_sire_reg_number UNIQUE (dam_full_reg_number, sire_full_reg_number);


--
-- Name: commodity_report commodity_report_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.commodity_report
    ADD CONSTRAINT commodity_report_pkey PRIMARY KEY (event_time);


--
-- Name: credit credit_event_time_event_type_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.credit
    ADD CONSTRAINT credit_event_time_event_type_key UNIQUE (event_time, event_type);


--
-- Name: credit_exchange credit_exchange_date_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.credit_exchange
    ADD CONSTRAINT credit_exchange_date_key UNIQUE (date);


--
-- Name: credit_exchange credit_exchange_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.credit_exchange
    ADD CONSTRAINT credit_exchange_pkey PRIMARY KEY (date);


--
-- Name: daily_number_cows_milking daily_number_cows_milking_date_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.daily_number_cows_milking
    ADD CONSTRAINT daily_number_cows_milking_date_key UNIQUE (date);


--
-- Name: daily_number_cows_milking daily_number_cows_milking_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.daily_number_cows_milking
    ADD CONSTRAINT daily_number_cows_milking_pkey PRIMARY KEY (id);


--
-- Name: day_ingredient_usage_result day_ingredient_usage_result_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.day_ingredient_usage_result
    ADD CONSTRAINT day_ingredient_usage_result_pkey PRIMARY KEY (feedcurr_id, date);


--
-- Name: employee_shift employee_shift_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.employee_shift
    ADD CONSTRAINT employee_shift_pkey PRIMARY KEY (userid, date, shift);


--
-- Name: error_curr error_curr_hash_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.error_curr
    ADD CONSTRAINT error_curr_hash_key UNIQUE (hash);


--
-- Name: error_curr error_curr_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.error_curr
    ADD CONSTRAINT error_curr_pkey PRIMARY KEY (id);


--
-- Name: holstein_canada_data holstein_canada_data_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.holstein_canada_data
    ADD CONSTRAINT holstein_canada_data_pkey PRIMARY KEY (reg_no, extract_date);


--
-- Name: holstein_canada_registered holstein_canada_registered_bovine_id_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.holstein_canada_registered
    ADD CONSTRAINT holstein_canada_registered_bovine_id_key UNIQUE (bovine_id);


--
-- Name: holstein_canada_registered holstein_canada_registered_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.holstein_canada_registered
    ADD CONSTRAINT holstein_canada_registered_pkey PRIMARY KEY (bovine_id);


--
-- Name: holstein_canada_registered holstein_canada_registered_short_uuid_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.holstein_canada_registered
    ADD CONSTRAINT holstein_canada_registered_short_uuid_key UNIQUE (short_uuid);


--
-- Name: incentive_day incentive_day_event_time_incentive_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.incentive_day
    ADD CONSTRAINT incentive_day_event_time_incentive_key UNIQUE (date, incentive);


--
-- Name: incentive_day incentive_day_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.incentive_day
    ADD CONSTRAINT incentive_day_pkey PRIMARY KEY (id);


--
-- Name: milk2020_hoof milk2020_hoof_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.milk2020_hoof
    ADD CONSTRAINT milk2020_hoof_pkey PRIMARY KEY (chain_number, trim_time);


--
-- Name: milk_pickup_event milk_pickup_event_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.milk_pickup_event
    ADD CONSTRAINT milk_pickup_event_pkey PRIMARY KEY (id);


--
-- Name: milk_statement milk_statement_date_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.milk_statement
    ADD CONSTRAINT milk_statement_date_key UNIQUE (date);


--
-- Name: nb_bulk_tank_health nb_bulk_tank_health_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.nb_bulk_tank_health
    ADD CONSTRAINT nb_bulk_tank_health_pkey PRIMARY KEY (id);


--
-- Name: nb_bulk_tank_sample provincial_milk_test_data_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.nb_bulk_tank_sample
    ADD CONSTRAINT provincial_milk_test_data_pkey PRIMARY KEY (id);


--
-- Name: quota quota_event_time_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.quota
    ADD CONSTRAINT quota_event_time_key UNIQUE (event_time);

ALTER TABLE batch.quota CLUSTER ON quota_event_time_key;


--
-- Name: quota quota_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.quota
    ADD CONSTRAINT quota_pkey PRIMARY KEY (id);


--
-- Name: shurgain_invoice_sheet1 shurgain_invoice_sheet1_data_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.shurgain_invoice_sheet1
    ADD CONSTRAINT shurgain_invoice_sheet1_data_key UNIQUE (data);


--
-- Name: valacta_group_averages valacta_group_averages_event_time_key; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.valacta_group_averages
    ADD CONSTRAINT valacta_group_averages_event_time_key UNIQUE (event_time, location_id);


--
-- Name: valacta_raw valacta_raw_pkey; Type: CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.valacta_raw
    ADD CONSTRAINT valacta_raw_pkey PRIMARY KEY (chain_number, test_date);


--
-- Name: body_condition_score_event body_condition_score_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.body_condition_score_event
    ADD CONSTRAINT body_condition_score_event_pkey PRIMARY KEY (id);


--
-- Name: location_sort bovine_estimated_sort_time; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_sort
    ADD CONSTRAINT bovine_estimated_sort_time UNIQUE (bovine_id, estimated_sort_time);


--
-- Name: breeding_event breeding_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.breeding_event
    ADD CONSTRAINT breeding_event_pkey PRIMARY KEY (id);


--
-- Name: protocol_event breeding_protocol_event_pkey1; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.protocol_event
    ADD CONSTRAINT breeding_protocol_event_pkey1 PRIMARY KEY (id);


--
-- Name: protocol_event breeding_protocol_event_uuid_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.protocol_event
    ADD CONSTRAINT breeding_protocol_event_uuid_key UNIQUE (protocol_uuid);


--
-- Name: protocol_type breeding_protocol_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.protocol_type
    ADD CONSTRAINT breeding_protocol_pkey PRIMARY KEY (id);


--
-- Name: calf_milk_administered calf_milk_administered_calendar_event_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calf_milk_administered
    ADD CONSTRAINT calf_milk_administered_calendar_event_id_key UNIQUE (calendar_event_id);


--
-- Name: calf_milk_administered calf_milk_administered_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calf_milk_administered
    ADD CONSTRAINT calf_milk_administered_pkey PRIMARY KEY (id);


--
-- Name: calf_milk calf_milk_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calf_milk
    ADD CONSTRAINT calf_milk_pkey PRIMARY KEY (id);


--
-- Name: calf_potential_name calf_potential_name_bovine_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calf_potential_name
    ADD CONSTRAINT calf_potential_name_bovine_id_key UNIQUE (bovine_id);


--
-- Name: calving_event calving stats_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event
    ADD CONSTRAINT "calving stats_pkey" PRIMARY KEY (id);


--
-- Name: calving_event calving_event_calf_bovine_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event
    ADD CONSTRAINT calving_event_calf_bovine_id_key UNIQUE (calf_bovine_id);


--
-- Name: calving_event calving_event_calf_full_reg_number_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event
    ADD CONSTRAINT calving_event_calf_full_reg_number_key UNIQUE (calf_rfid_number);


--
-- Name: calving_event_ease_types calving_stats_ease_types_name_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event_ease_types
    ADD CONSTRAINT calving_stats_ease_types_name_key UNIQUE (name);


--
-- Name: calving_event_ease_types calving_stats_ease_types_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event_ease_types
    ADD CONSTRAINT calving_stats_ease_types_pkey PRIMARY KEY (id);


--
-- Name: breeding_event combined_bovine_id_actual_breeding_time; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.breeding_event
    ADD CONSTRAINT combined_bovine_id_actual_breeding_time UNIQUE (bovine_id, actual_breeding_time);


--
-- Name: preg_check_event combined_bovine_id_event_time; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_event
    ADD CONSTRAINT combined_bovine_id_event_time UNIQUE (bovine_id, event_time);


--
-- Name: location_event combined_location_bovine_id_event_time; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_event
    ADD CONSTRAINT combined_location_bovine_id_event_time UNIQUE (bovine_id, event_time);

ALTER TABLE bovinemanagement.location_event CLUSTER ON combined_location_bovine_id_event_time;


--
-- Name: comment_custom comment_reproductive_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.comment_custom
    ADD CONSTRAINT comment_reproductive_pkey PRIMARY KEY (id);


--
-- Name: cull_event cull_event_bovine_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.cull_event
    ADD CONSTRAINT cull_event_bovine_id_key UNIQUE (bovine_id);


--
-- Name: cull_event cull_event_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.cull_event
    ADD CONSTRAINT cull_event_id_key UNIQUE (id);


--
-- Name: cull_list_event cull_list_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.cull_list_event
    ADD CONSTRAINT cull_list_event_pkey PRIMARY KEY (id);


--
-- Name: dehorn_event dehorn_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dehorn_event
    ADD CONSTRAINT dehorn_event_pkey PRIMARY KEY (id);


--
-- Name: dnatest_event dnatest_event_bovine_id_event_time_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dnatest_event
    ADD CONSTRAINT dnatest_event_bovine_id_event_time_key UNIQUE (bovine_id, event_time);


--
-- Name: dnatest_event dnatest_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dnatest_event
    ADD CONSTRAINT dnatest_event_pkey PRIMARY KEY (id);


--
-- Name: dryoff_event dryoff_event_event_time_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_event_time_key UNIQUE (event_time, lactation_id);


--
-- Name: dryoff_event dryoff_event_lactation_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_lactation_id_key UNIQUE (lactation_id);


--
-- Name: eartag eartag_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.eartag
    ADD CONSTRAINT eartag_pkey PRIMARY KEY (id);


--
-- Name: eartag_valid eartag_valid_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.eartag_valid
    ADD CONSTRAINT eartag_valid_pkey PRIMARY KEY (rfid_range);


--
-- Name: embryo_flush embryo_flush_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_flush
    ADD CONSTRAINT embryo_flush_pkey PRIMARY KEY (id);


--
-- Name: embryo_implant_comment embryo_implant_comment_comment_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant_comment
    ADD CONSTRAINT embryo_implant_comment_comment_key UNIQUE (comment);


--
-- Name: embryo_implant_comment embryo_implant_comment_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant_comment
    ADD CONSTRAINT embryo_implant_comment_pkey PRIMARY KEY (id);


--
-- Name: embryo_implant embryo_recipient_embryo_straw_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant
    ADD CONSTRAINT embryo_recipient_embryo_straw_id_key UNIQUE (embryo_straw_id);


--
-- Name: embryo_implant embryo_recipient_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant
    ADD CONSTRAINT embryo_recipient_pkey PRIMARY KEY (id);


--
-- Name: embryo_straw embryo_straw_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_straw
    ADD CONSTRAINT embryo_straw_pkey PRIMARY KEY (id);


--
-- Name: estrus_type estrus_event_display_order_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_type
    ADD CONSTRAINT estrus_event_display_order_key UNIQUE (display_order);


--
-- Name: estrus_type estrus_event_name_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_type
    ADD CONSTRAINT estrus_event_name_key UNIQUE (name);


--
-- Name: estrus_type estrus_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_type
    ADD CONSTRAINT estrus_event_pkey PRIMARY KEY (id);

ALTER TABLE bovinemanagement.estrus_type CLUSTER ON estrus_event_pkey;


--
-- Name: estrus_event estrus_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_event
    ADD CONSTRAINT estrus_id_key UNIQUE (id);


--
-- Name: estrus_event estrus_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_event
    ADD CONSTRAINT estrus_pkey PRIMARY KEY (id);


--
-- Name: foot_event foot_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.foot_event
    ADD CONSTRAINT foot_event_pkey PRIMARY KEY (id);


--
-- Name: foot_type foot_type_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.foot_type
    ADD CONSTRAINT foot_type_pkey PRIMARY KEY (id);


--
-- Name: kamar_event kamar_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.kamar_event
    ADD CONSTRAINT kamar_event_pkey PRIMARY KEY (id);


--
-- Name: lactation lactation_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.lactation
    ADD CONSTRAINT lactation_pkey PRIMARY KEY (id);


--
-- Name: location_event location_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_event
    ADD CONSTRAINT location_event_pkey PRIMARY KEY (id);


--
-- Name: location location_name_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location
    ADD CONSTRAINT location_name_key UNIQUE (name);


--
-- Name: location location_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location
    ADD CONSTRAINT location_pkey PRIMARY KEY (id);

ALTER TABLE bovinemanagement.location CLUSTER ON location_pkey;


--
-- Name: location_sort location_sort_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_sort
    ADD CONSTRAINT location_sort_pkey PRIMARY KEY (id);


--
-- Name: location_urban_feedstall location_urban_feedstall_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_urban_feedstall
    ADD CONSTRAINT location_urban_feedstall_pkey PRIMARY KEY (location_id, feedstall_id);


--
-- Name: medical_action_type medical_action_type_diagnosis_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_action_type
    ADD CONSTRAINT medical_action_type_diagnosis_key UNIQUE (action);


--
-- Name: medical_action_type medical_action_type_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_action_type
    ADD CONSTRAINT medical_action_type_pkey PRIMARY KEY (id);


--
-- Name: medical_case medical_case_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_case
    ADD CONSTRAINT medical_case_pkey PRIMARY KEY (id);


--
-- Name: medical_comment medical_comment_bovine_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_comment
    ADD CONSTRAINT medical_comment_bovine_id_key UNIQUE (bovine_id, event_time);


--
-- Name: medical_comment medical_comment_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_comment
    ADD CONSTRAINT medical_comment_pkey PRIMARY KEY (id);


--
-- Name: medical_diagnosis medical_diagnosis_bovine_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_diagnosis
    ADD CONSTRAINT medical_diagnosis_bovine_id_key UNIQUE (bovine_id, event_time);


--
-- Name: medical_diagnosis_type medical_diagnosis_diagnosis_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_diagnosis_type
    ADD CONSTRAINT medical_diagnosis_diagnosis_key UNIQUE (diagnosis);


--
-- Name: medical_diagnosis_type medical_diagnosis_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_diagnosis_type
    ADD CONSTRAINT medical_diagnosis_pkey PRIMARY KEY (id);

ALTER TABLE bovinemanagement.medical_diagnosis_type CLUSTER ON medical_diagnosis_pkey;


--
-- Name: medical_diagnosis medical_diagnosis_pkey1; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_diagnosis
    ADD CONSTRAINT medical_diagnosis_pkey1 PRIMARY KEY (id);


--
-- Name: medical_ketone medical_ketone_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_ketone
    ADD CONSTRAINT medical_ketone_pkey PRIMARY KEY (id);


--
-- Name: medical_magnet medical_magnet_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_magnet
    ADD CONSTRAINT medical_magnet_pkey PRIMARY KEY (id);


--
-- Name: medical_temperature medical_temperature_bovine_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_temperature
    ADD CONSTRAINT medical_temperature_bovine_id_key UNIQUE (bovine_id, event_time);


--
-- Name: medical_temperature medical_temperature_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_temperature
    ADD CONSTRAINT medical_temperature_pkey PRIMARY KEY (id);


--
-- Name: medical_action medicine_action_calendar_event_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_action
    ADD CONSTRAINT medicine_action_calendar_event_id_key UNIQUE (calendar_event_id);


--
-- Name: medical_action medicine_action_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_action
    ADD CONSTRAINT medicine_action_pkey PRIMARY KEY (id);


--
-- Name: medicine_administered medicine_administered_calendar_event_id_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medicine_administered
    ADD CONSTRAINT medicine_administered_calendar_event_id_key UNIQUE (calendar_event_id);


--
-- Name: medicine_administered medicine_administered_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medicine_administered
    ADD CONSTRAINT medicine_administered_pkey PRIMARY KEY (id);


--
-- Name: medicine medicine_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medicine
    ADD CONSTRAINT medicine_pkey PRIMARY KEY (id);


--
-- Name: medicine medicine_trade_name_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medicine
    ADD CONSTRAINT medicine_trade_name_key UNIQUE (trade_name);


--
-- Name: palpate_comment palpate_comment_comment_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.palpate_comment
    ADD CONSTRAINT palpate_comment_comment_key UNIQUE (comment);


--
-- Name: palpate_comment palpate_comment_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.palpate_comment
    ADD CONSTRAINT palpate_comment_pkey PRIMARY KEY (id);


--
-- Name: preg_check_comment preg_check_comment_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_comment
    ADD CONSTRAINT preg_check_comment_pkey PRIMARY KEY (id);


--
-- Name: preg_check_event preg_check_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_event
    ADD CONSTRAINT preg_check_event_pkey PRIMARY KEY (id);


--
-- Name: preg_check_type preg_check_type_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_type
    ADD CONSTRAINT preg_check_type_pkey PRIMARY KEY (id);


--
-- Name: bovine primary_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.bovine
    ADD CONSTRAINT primary_key PRIMARY KEY (id);


--
-- Name: production_item production_item_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.production_item
    ADD CONSTRAINT production_item_pkey PRIMARY KEY (id);


--
-- Name: sale_price_comment sale_price_comment_comment_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sale_price_comment
    ADD CONSTRAINT sale_price_comment_comment_key UNIQUE (comment);


--
-- Name: sale_price_comment sale_price_comment_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sale_price_comment
    ADD CONSTRAINT sale_price_comment_pkey PRIMARY KEY (id);


--
-- Name: sale_price sale_price_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sale_price
    ADD CONSTRAINT sale_price_pkey PRIMARY KEY (id);


--
-- Name: semen_straw semen_straw_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.semen_straw
    ADD CONSTRAINT semen_straw_pkey PRIMARY KEY (id);


--
-- Name: service_picks service_picks_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.service_picks
    ADD CONSTRAINT service_picks_pkey PRIMARY KEY (id);


--
-- Name: sire sire_full_name_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sire
    ADD CONSTRAINT sire_full_name_key UNIQUE (full_name);


--
-- Name: sire sire_full_reg_number_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sire
    ADD CONSTRAINT sire_full_reg_number_key UNIQUE (full_reg_number);


--
-- Name: sire sire_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sire
    ADD CONSTRAINT sire_pkey PRIMARY KEY (id);


--
-- Name: sire_semen_code sire_semen_code_semen_code_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sire_semen_code
    ADD CONSTRAINT sire_semen_code_semen_code_key UNIQUE (semen_code);

ALTER TABLE bovinemanagement.sire_semen_code CLUSTER ON sire_semen_code_semen_code_key;


--
-- Name: sale_price unique_bovine_id_event_time; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sale_price
    ADD CONSTRAINT unique_bovine_id_event_time UNIQUE (bovine_id, event_time);


--
-- Name: bovine unique_full_reg; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.bovine
    ADD CONSTRAINT unique_full_reg UNIQUE (full_reg_number);


--
-- Name: bovine unique_local; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.bovine
    ADD CONSTRAINT unique_local UNIQUE (local_number);


--
-- Name: bovine unique_rfid; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.bovine
    ADD CONSTRAINT unique_rfid UNIQUE (rfid_number);


--
-- Name: vet_to_check_event vet_to_check_event_bovine_id_event_time_key; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.vet_to_check_event
    ADD CONSTRAINT vet_to_check_event_bovine_id_event_time_key UNIQUE (bovine_id, event_time);


--
-- Name: vet_to_check_event vet_to_check_event_pkey; Type: CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.vet_to_check_event
    ADD CONSTRAINT vet_to_check_event_pkey PRIMARY KEY (id);


--
-- Name: border_event border_field_id_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.border_event
    ADD CONSTRAINT border_field_id_key UNIQUE (field_id, event_time);


--
-- Name: border_event border_field_id_key1; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.border_event
    ADD CONSTRAINT border_field_id_key1 UNIQUE (field_id, datum_id);


--
-- Name: border_event border_id_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.border_event
    ADD CONSTRAINT border_id_key UNIQUE (id);


--
-- Name: border_event border_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.border_event
    ADD CONSTRAINT border_pkey PRIMARY KEY (id);


--
-- Name: comment_event comment_event_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.comment_event
    ADD CONSTRAINT comment_event_pkey PRIMARY KEY (id);


--
-- Name: datum datum_pkey1; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.datum
    ADD CONSTRAINT datum_pkey1 PRIMARY KEY (id);


--
-- Name: fertilizer_event fertilizer_event_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.fertilizer_event
    ADD CONSTRAINT fertilizer_event_pkey PRIMARY KEY (id);


--
-- Name: fertilizer_event_scheduled fertilizer_event_scheduled_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.fertilizer_event_scheduled
    ADD CONSTRAINT fertilizer_event_scheduled_pkey PRIMARY KEY (id);


--
-- Name: fertilizer fertilizer_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.fertilizer
    ADD CONSTRAINT fertilizer_pkey PRIMARY KEY (id);


--
-- Name: fertilizer_recommendation_crop fertilizer_recommendation_cro_specific_type_fertilizer_reco_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.fertilizer_recommendation_crop
    ADD CONSTRAINT fertilizer_recommendation_cro_specific_type_fertilizer_reco_key UNIQUE (specific_type, fertilizer_recommendation_id);


--
-- Name: fertilizer_recommendation fertilizer_recommendation_id_element_element_range_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.fertilizer_recommendation
    ADD CONSTRAINT fertilizer_recommendation_id_element_element_range_key UNIQUE (id, element, element_range);


--
-- Name: fertilizer_removal_rate fertilizer_removal_rate_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.fertilizer_removal_rate
    ADD CONSTRAINT fertilizer_removal_rate_pkey PRIMARY KEY (crop);


--
-- Name: fertilizer_application_method fertilzer_application_method_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.fertilizer_application_method
    ADD CONSTRAINT fertilzer_application_method_pkey PRIMARY KEY (id);


--
-- Name: field field_alpha_numeric_id_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.field
    ADD CONSTRAINT field_alpha_numeric_id_key UNIQUE (alpha_numeric_id);


--
-- Name: soil_sample_event field_event_unique; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.soil_sample_event
    ADD CONSTRAINT field_event_unique UNIQUE (field_id, event_time);


--
-- Name: field_parameter field_parameters_field_id_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.field_parameter
    ADD CONSTRAINT field_parameters_field_id_key UNIQUE (field_id, event_time);


--
-- Name: field_parameter field_parameters_id_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.field_parameter
    ADD CONSTRAINT field_parameters_id_key UNIQUE (id);


--
-- Name: field_parameter field_parameters_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.field_parameter
    ADD CONSTRAINT field_parameters_pkey PRIMARY KEY (id);


--
-- Name: field field_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.field
    ADD CONSTRAINT field_pkey PRIMARY KEY (id);


--
-- Name: general_comment_event general_comment_event_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.general_comment_event
    ADD CONSTRAINT general_comment_event_pkey PRIMARY KEY (id);


--
-- Name: harvest_event harvest_event_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.harvest_event
    ADD CONSTRAINT harvest_event_pkey PRIMARY KEY (id);


--
-- Name: lime_event lime_event_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.lime_event
    ADD CONSTRAINT lime_event_pkey PRIMARY KEY (id);


--
-- Name: lime lime_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.lime
    ADD CONSTRAINT lime_pkey PRIMARY KEY (id);


--
-- Name: manure_event manure_event_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.manure_event
    ADD CONSTRAINT manure_event_pkey PRIMARY KEY (id);


--
-- Name: plant_tissue_parameter plant_tissue_parameter_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.plant_tissue_parameter
    ADD CONSTRAINT plant_tissue_parameter_pkey PRIMARY KEY (id);


--
-- Name: seed_category seed_category_id_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed_category
    ADD CONSTRAINT seed_category_id_key UNIQUE (id);


--
-- Name: seed_event seed_event_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed_event
    ADD CONSTRAINT seed_event_pkey PRIMARY KEY (id);


--
-- Name: seed_event_scheduled seed_event_scheduled_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed_event_scheduled
    ADD CONSTRAINT seed_event_scheduled_pkey PRIMARY KEY (id);


--
-- Name: seed_inventory seed_inventory_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed_inventory
    ADD CONSTRAINT seed_inventory_pkey PRIMARY KEY (id);


--
-- Name: seed seed_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed
    ADD CONSTRAINT seed_pkey PRIMARY KEY (id);


--
-- Name: soil_sample_event soil_sample_event_id_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.soil_sample_event
    ADD CONSTRAINT soil_sample_event_id_key UNIQUE (id);


--
-- Name: spray_event spray_event_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.spray_event
    ADD CONSTRAINT spray_event_pkey PRIMARY KEY (id);


--
-- Name: spray spray_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.spray
    ADD CONSTRAINT spray_pkey PRIMARY KEY (id);


--
-- Name: tillage_event tillage_event_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.tillage_event
    ADD CONSTRAINT tillage_event_pkey PRIMARY KEY (id);


--
-- Name: tillage tillage_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.tillage
    ADD CONSTRAINT tillage_pkey PRIMARY KEY (id);


--
-- Name: yield_event yield_event_id_key; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.yield_event
    ADD CONSTRAINT yield_event_id_key UNIQUE (id);


--
-- Name: yield_type yield_type_pkey; Type: CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.yield_type
    ADD CONSTRAINT yield_type_pkey PRIMARY KEY (id);


--
-- Name: document document_pkey; Type: CONSTRAINT; Schema: documents; Owner: -
--

ALTER TABLE ONLY documents.document
    ADD CONSTRAINT document_pkey PRIMARY KEY (id);


--
-- Name: coverage coverage_pkey; Type: CONSTRAINT; Schema: gis; Owner: -
--

ALTER TABLE ONLY gis.coverage
    ADD CONSTRAINT coverage_pkey PRIMARY KEY (gid);


--
-- Name: roadstable roadstable_pkey; Type: CONSTRAINT; Schema: gis; Owner: -
--

ALTER TABLE ONLY gis.roadstable
    ADD CONSTRAINT roadstable_pkey PRIMARY KEY (gid);


--
-- Name: summary summary_pkey; Type: CONSTRAINT; Schema: gis; Owner: -
--

ALTER TABLE ONLY gis.summary
    ADD CONSTRAINT summary_pkey PRIMARY KEY (id);


--
-- Name: swaths swaths_pkey; Type: CONSTRAINT; Schema: gis; Owner: -
--

ALTER TABLE ONLY gis.swaths
    ADD CONSTRAINT swaths_pkey PRIMARY KEY (gid);


--
-- Name: overtime overtime_event_date_key; Type: CONSTRAINT; Schema: hr; Owner: -
--

ALTER TABLE ONLY hr.overtime
    ADD CONSTRAINT overtime_event_date_key UNIQUE (event_date, overtime_userid);


--
-- Name: overtime overtime_pkey; Type: CONSTRAINT; Schema: hr; Owner: -
--

ALTER TABLE ONLY hr.overtime
    ADD CONSTRAINT overtime_pkey PRIMARY KEY (id);


--
-- Name: ajax ajax_pkey; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.ajax
    ADD CONSTRAINT ajax_pkey PRIMARY KEY (id);

ALTER TABLE intwebsite.ajax CLUSTER ON ajax_pkey;


--
-- Name: ajax_security ajax_security_pkey; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.ajax_security
    ADD CONSTRAINT ajax_security_pkey PRIMARY KEY (id);


--
-- Name: page page_pageid_key; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.page
    ADD CONSTRAINT page_pageid_key UNIQUE (pageid);


--
-- Name: page page_pkey; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.page
    ADD CONSTRAINT page_pkey PRIMARY KEY (pageid);


--
-- Name: page_security page_security_group_key; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.page_security
    ADD CONSTRAINT page_security_group_key UNIQUE ("group", pageid);


--
-- Name: page_security page_security_pkey; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.page_security
    ADD CONSTRAINT page_security_pkey PRIMARY KEY (id);


--
-- Name: page page_title_key; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.page
    ADD CONSTRAINT page_title_key UNIQUE (title);


--
-- Name: sse sse_id_key; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.sse
    ADD CONSTRAINT sse_id_key UNIQUE (id);


--
-- Name: sse sse_pkey; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.sse
    ADD CONSTRAINT sse_pkey PRIMARY KEY (id);


--
-- Name: sse_security sse_security_pkey; Type: CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.sse_security
    ADD CONSTRAINT sse_security_pkey PRIMARY KEY (id);


--
-- Name: comment comment_pkey; Type: CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.comment
    ADD CONSTRAINT comment_pkey PRIMARY KEY (id);


--
-- Name: hours_log hours_log_id_key; Type: CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.hours_log
    ADD CONSTRAINT hours_log_id_key UNIQUE (id);


--
-- Name: hours_log hours_log_machine_id_key; Type: CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.hours_log
    ADD CONSTRAINT hours_log_machine_id_key UNIQUE (machine_id, hours);


--
-- Name: machine machine_pkey; Type: CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.machine
    ADD CONSTRAINT machine_pkey PRIMARY KEY (id);


--
-- Name: service_administered service_administered_service_item_id_key; Type: CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.service_administered
    ADD CONSTRAINT service_administered_service_item_id_key UNIQUE (service_item_id, event_time);


--
-- Name: service_item service_item_pkey; Type: CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.service_item
    ADD CONSTRAINT service_item_pkey PRIMARY KEY (id);


--
-- Name: chat_text chat_text_pkey; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.chat_text
    ADD CONSTRAINT chat_text_pkey PRIMARY KEY (create_time, userid);


--
-- Name: cqm_record13 cqm_record13_pkey; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.cqm_record13
    ADD CONSTRAINT cqm_record13_pkey PRIMARY KEY (id);


--
-- Name: cqm_record17 cqm_record17_pkey; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.cqm_record17
    ADD CONSTRAINT cqm_record17_pkey PRIMARY KEY (id);


--
-- Name: cqm_record9 cqm_record9_pkey; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.cqm_record9
    ADD CONSTRAINT cqm_record9_pkey PRIMARY KEY (id);


--
-- Name: main main_pkey; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.main
    ADD CONSTRAINT main_pkey PRIMARY KEY (generate_dates);


--
-- Name: purchase_suppliers purchase_suppliers_pkey; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.purchase_suppliers
    ADD CONSTRAINT purchase_suppliers_pkey PRIMARY KEY (name);


--
-- Name: purchases purchases_pkey; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.purchases
    ADD CONSTRAINT purchases_pkey PRIMARY KEY (id);


--
-- Name: sop_content sop_content_sop_id_key; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.sop_content
    ADD CONSTRAINT sop_content_sop_id_key UNIQUE (sop_id, event_time);


--
-- Name: sop sop_pkey; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.sop
    ADD CONSTRAINT sop_pkey PRIMARY KEY (id);


--
-- Name: sop sop_title_key; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.sop
    ADD CONSTRAINT sop_title_key UNIQUE (title);


--
-- Name: task_completed task_completed_task_id_key; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.task_completed
    ADD CONSTRAINT task_completed_task_id_key UNIQUE (task_id, event_time);


--
-- Name: task task_pkey; Type: CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.task
    ADD CONSTRAINT task_pkey PRIMARY KEY (id);


--
-- Name: recipe 3_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.recipe
    ADD CONSTRAINT "3_pkey" PRIMARY KEY (id);

ALTER TABLE nutrition.recipe CLUSTER ON "3_pkey";


--
-- Name: analysis_link analysis_link_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.analysis_link
    ADD CONSTRAINT analysis_link_pkey PRIMARY KEY (id);


--
-- Name: bag_analysis bag_analysis2_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_analysis
    ADD CONSTRAINT bag_analysis2_pkey PRIMARY KEY (id);


--
-- Name: bag_analysis bag_analysis2_sample_number_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_analysis
    ADD CONSTRAINT bag_analysis2_sample_number_key UNIQUE (sample_number);


--
-- Name: bag_comment bag_comment_bag_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_comment
    ADD CONSTRAINT bag_comment_bag_id_key UNIQUE (bag_id, footage);


--
-- Name: bag_comment bag_comment_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_comment
    ADD CONSTRAINT bag_comment_pkey PRIMARY KEY (id);


--
-- Name: bag_consumption bag_consumption_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_consumption
    ADD CONSTRAINT bag_consumption_pkey PRIMARY KEY (id);


--
-- Name: bag_cost bag_cost_bag_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_cost
    ADD CONSTRAINT bag_cost_bag_id_key UNIQUE (bag_id, footage_start);


--
-- Name: bag_cost bag_cost_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_cost
    ADD CONSTRAINT bag_cost_pkey PRIMARY KEY (id);


--
-- Name: bag_ensile_date bag_ensile_date_bag_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_ensile_date
    ADD CONSTRAINT bag_ensile_date_bag_id_key UNIQUE (bag_id, footage);


--
-- Name: bag_ensile_date bag_ensile_date_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_ensile_date
    ADD CONSTRAINT bag_ensile_date_pkey PRIMARY KEY (id);


--
-- Name: bag bag_ensiled_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag
    ADD CONSTRAINT bag_ensiled_pkey PRIMARY KEY (id);

ALTER TABLE nutrition.bag CLUSTER ON bag_ensiled_pkey;


--
-- Name: bag_feed bag_feed_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_feed
    ADD CONSTRAINT bag_feed_pkey PRIMARY KEY (id);


--
-- Name: bag_field bag_field_bag_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_field
    ADD CONSTRAINT bag_field_bag_id_key UNIQUE (bag_id, field_id, footage_start);


--
-- Name: bag_field bag_field_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_field
    ADD CONSTRAINT bag_field_pkey PRIMARY KEY (id);


--
-- Name: bag_moisture_test bag_moisture_test_bag_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_moisture_test
    ADD CONSTRAINT bag_moisture_test_bag_id_key UNIQUE (bag_id, footage);


--
-- Name: bag_moisture_test bag_moisture_test_bag_id_key1; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_moisture_test
    ADD CONSTRAINT bag_moisture_test_bag_id_key1 UNIQUE (bag_id, event_time);


--
-- Name: bag_moisture_test bag_moisture_test_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_moisture_test
    ADD CONSTRAINT bag_moisture_test_pkey PRIMARY KEY (id);


--
-- Name: feed_cost feed_cost_feed_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_cost
    ADD CONSTRAINT feed_cost_feed_id_key UNIQUE (feed_id, event_time);


--
-- Name: feed_library_nrc2001 feed_library_Feed Name_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_library_nrc2001
    ADD CONSTRAINT "feed_library_Feed Name_key" UNIQUE ("Feed Name");


--
-- Name: feed_library_local feed_library_local_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_library_local
    ADD CONSTRAINT feed_library_local_pkey PRIMARY KEY ("Feed Name");


--
-- Name: feed_library_nrc2001 feed_library_nrc2001_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_library_nrc2001
    ADD CONSTRAINT feed_library_nrc2001_pkey PRIMARY KEY ("Feed Name");


--
-- Name: feed_location feed_location_location_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_location
    ADD CONSTRAINT feed_location_location_key UNIQUE (location);


--
-- Name: feed_location feed_location_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_location
    ADD CONSTRAINT feed_location_pkey PRIMARY KEY (id);


--
-- Name: feed_moisture feed_moisture_feed_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_moisture
    ADD CONSTRAINT feed_moisture_feed_id_key UNIQUE (feed_id, date);


--
-- Name: feed_type feed_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_type
    ADD CONSTRAINT feed_pkey PRIMARY KEY (id);


--
-- Name: feed feed_pkey1; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed
    ADD CONSTRAINT feed_pkey1 PRIMARY KEY (id);


--
-- Name: mix_modify mix_modify_mix_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.mix_modify
    ADD CONSTRAINT mix_modify_mix_id_key UNIQUE (mix_id, date);


--
-- Name: mix mix_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.mix
    ADD CONSTRAINT mix_pkey PRIMARY KEY (id);


--
-- Name: mix mix_recipe_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.mix
    ADD CONSTRAINT mix_recipe_id_key UNIQUE (recipe_id, location_id);


--
-- Name: nrc_archive nrc_archive_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_archive
    ADD CONSTRAINT nrc_archive_pkey PRIMARY KEY (nrc_recipe_id);


--
-- Name: nrc_recipe_animal_input nrc_recipe_animal_input_nrc_recipe_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_animal_input
    ADD CONSTRAINT nrc_recipe_animal_input_nrc_recipe_key UNIQUE (nrc_recipe);


--
-- Name: nrc_recipe_animal_input nrc_recipe_animal_input_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_animal_input
    ADD CONSTRAINT nrc_recipe_animal_input_pkey PRIMARY KEY (nrc_recipe);


--
-- Name: nrc_recipe_item_feed_log nrc_recipe_feed_log_nrc_recipe_item_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_item_feed_log
    ADD CONSTRAINT nrc_recipe_feed_log_nrc_recipe_item_id_key UNIQUE (nrc_recipe_item_id);


--
-- Name: nrc_recipe_item nrc_recipe_item_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_item
    ADD CONSTRAINT nrc_recipe_item_id_key UNIQUE (id);


--
-- Name: nrc_recipe_item nrc_recipe_item_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_item
    ADD CONSTRAINT nrc_recipe_item_pkey PRIMARY KEY (nrc_recipe, feed_library_name);


--
-- Name: nrc_recipe_location nrc_recipe_location_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_location
    ADD CONSTRAINT nrc_recipe_location_pkey PRIMARY KEY (nrc_recipe, location_id);


--
-- Name: nrc_recipe_param nrc_recipe_param_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_param
    ADD CONSTRAINT nrc_recipe_param_pkey PRIMARY KEY (nrc_recipe);


--
-- Name: recipe_item recipe_item_feedcurr_id_key; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.recipe_item
    ADD CONSTRAINT recipe_item_feedcurr_id_key UNIQUE (feedcurr_id, recipe_id);


--
-- Name: recipe_item recipe_item_pkey; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.recipe_item
    ADD CONSTRAINT recipe_item_pkey PRIMARY KEY (id);


--
-- Name: bag_consumption unique_bag_footage; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_consumption
    ADD CONSTRAINT unique_bag_footage UNIQUE (footage, bag_id, direction);


--
-- Name: bag_feed unique_bag_footages; Type: CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_feed
    ADD CONSTRAINT unique_bag_footages UNIQUE (bag_id, footage_start, footage_finish);


--
-- Name: bovine bovine_bovine_id_key; Type: CONSTRAINT; Schema: picture; Owner: -
--

ALTER TABLE ONLY picture.bovine
    ADD CONSTRAINT bovine_bovine_id_key UNIQUE (bovine_id, event_time);


--
-- Name: bovine bovine_pkey; Type: CONSTRAINT; Schema: picture; Owner: -
--

ALTER TABLE ONLY picture.bovine
    ADD CONSTRAINT bovine_pkey PRIMARY KEY (id);


--
-- Name: machine general_pkey; Type: CONSTRAINT; Schema: picture; Owner: -
--

ALTER TABLE ONLY picture.machine
    ADD CONSTRAINT general_pkey PRIMARY KEY (id);


--
-- Name: machine_comment machine_comment_pkey; Type: CONSTRAINT; Schema: picture; Owner: -
--

ALTER TABLE ONLY picture.machine_comment
    ADD CONSTRAINT machine_comment_pkey PRIMARY KEY (id);


--
-- Name: classificationreport_temp classificationreport_temp_bovine_id_key; Type: CONSTRAINT; Schema: system; Owner: -
--

ALTER TABLE ONLY system.classificationreport_temp
    ADD CONSTRAINT classificationreport_temp_bovine_id_key UNIQUE (bovine_id);


--
-- Name: login_log login_log_pkey; Type: CONSTRAINT; Schema: system; Owner: -
--

ALTER TABLE ONLY system.login_log
    ADD CONSTRAINT login_log_pkey PRIMARY KEY (uuid);


--
-- Name: salesreport_temp salesreport_temp_bovine_id_key; Type: CONSTRAINT; Schema: system; Owner: -
--

ALTER TABLE ONLY system.salesreport_temp
    ADD CONSTRAINT salesreport_temp_bovine_id_key UNIQUE (bovine_id);


--
-- Name: update_log update_log_pkey; Type: CONSTRAINT; Schema: system; Owner: -
--

ALTER TABLE ONLY system.update_log
    ADD CONSTRAINT update_log_pkey PRIMARY KEY (schema_name, table_name, last_update);


--
-- Name: users_in_groups group_fk; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.users_in_groups
    ADD CONSTRAINT group_fk PRIMARY KEY (userid, groupid);


--
-- Name: groups groups_name_key; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.groups
    ADD CONSTRAINT groups_name_key UNIQUE (name);


--
-- Name: groups groups_pkey; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.groups
    ADD CONSTRAINT groups_pkey PRIMARY KEY (id);


--
-- Name: oauth_access_tokens oauth_access_token_pk; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.oauth_access_tokens
    ADD CONSTRAINT oauth_access_token_pk PRIMARY KEY (access_token);


--
-- Name: oauth_authorization_codes oauth_auth_code_pk; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.oauth_authorization_codes
    ADD CONSTRAINT oauth_auth_code_pk PRIMARY KEY (authorization_code);


--
-- Name: oauth_clients oauth_client_id_pk; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.oauth_clients
    ADD CONSTRAINT oauth_client_id_pk PRIMARY KEY (client_id);


--
-- Name: oauth_jwt oauth_client_id_pk2; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.oauth_jwt
    ADD CONSTRAINT oauth_client_id_pk2 PRIMARY KEY (client_id);


--
-- Name: oauth_refresh_tokens oauth_refresh_token_pk; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.oauth_refresh_tokens
    ADD CONSTRAINT oauth_refresh_token_pk PRIMARY KEY (refresh_token);


--
-- Name: oauth_scopes oauth_scopes_pkey; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.oauth_scopes
    ADD CONSTRAINT oauth_scopes_pkey PRIMARY KEY (scope, is_default);


--
-- Name: oauth_users oauth_username_pk; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.oauth_users
    ADD CONSTRAINT oauth_username_pk PRIMARY KEY (username);


--
-- Name: owners owners_pkey; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.owners
    ADD CONSTRAINT owners_pkey PRIMARY KEY (ownerid);


--
-- Name: users user_pk; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.users
    ADD CONSTRAINT user_pk PRIMARY KEY (userid);

ALTER TABLE wcauthentication.users CLUSTER ON user_pk;


--
-- Name: users users_email_key; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.users
    ADD CONSTRAINT users_email_key UNIQUE (email);


--
-- Name: users_google users_google_google_email_key; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.users_google
    ADD CONSTRAINT users_google_google_email_key UNIQUE (google_email);


--
-- Name: users_google users_google_pkey; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.users_google
    ADD CONSTRAINT users_google_pkey PRIMARY KEY (google_id);


--
-- Name: users usertable_userid_key; Type: CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.users
    ADD CONSTRAINT usertable_userid_key UNIQUE (userid);


--
-- Name: bovine_id_date_milking_number; Type: INDEX; Schema: alpro; Owner: -
--

CREATE UNIQUE INDEX bovine_id_date_milking_number ON alpro.cow USING btree (bovine_id, date, milking_number);

ALTER TABLE alpro.cow CLUSTER ON bovine_id_date_milking_number;


--
-- Name: idx_bovine_id; Type: INDEX; Schema: alpro; Owner: -
--

CREATE INDEX idx_bovine_id ON alpro.cow USING btree (bovine_id);


--
-- Name: idx_date; Type: INDEX; Schema: alpro; Owner: -
--

CREATE INDEX idx_date ON alpro.cow USING btree (date);


--
-- Name: idx_milking_number; Type: INDEX; Schema: alpro; Owner: -
--

CREATE INDEX idx_milking_number ON alpro.cow USING btree (milking_number);


--
-- Name: idx_milking_number_date; Type: INDEX; Schema: alpro; Owner: -
--

CREATE INDEX idx_milking_number_date ON alpro.cow USING btree (date, milking_number);


--
-- Name: sync_event_time_index; Type: INDEX; Schema: alpro; Owner: -
--

CREATE INDEX sync_event_time_index ON alpro.sync USING btree (event_time);


--
-- Name: Tablefeed_auger_event_range_event_range; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX "Tablefeed_auger_event_range_event_range" ON bas.feed_auger_event_range USING btree (event_range);

ALTER TABLE bas.feed_auger_event_range CLUSTER ON "Tablefeed_auger_event_range_event_range";


--
-- Name: ble_base_idx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX ble_base_idx ON bas.ble_base USING btree (base_id);


--
-- Name: ble_tag_event2_pkey; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX ble_tag_event2_pkey ON bas.ble_tag_event USING btree (tag_id, event_time DESC);

ALTER TABLE bas.ble_tag_event CLUSTER ON ble_tag_event2_pkey;


--
-- Name: ble_tag_event_filtered_variance_7day_event_time_idx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX ble_tag_event_filtered_variance_7day_event_time_idx ON bas.ble_tag_event_filtered_variance_7day USING btree (event_time);


--
-- Name: ble_tag_event_filtered_variance_7day_tag_id_idx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX ble_tag_event_filtered_variance_7day_tag_id_idx ON bas.ble_tag_event_filtered_variance_7day USING btree (tag_id);


--
-- Name: ble_tag_event_filtered_variance_event_time_idx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX ble_tag_event_filtered_variance_event_time_idx ON bas.ble_tag_event_filtered_variance_20min USING btree (event_time);


--
-- Name: ble_tag_event_filtered_variance_final_event_time_idx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX ble_tag_event_filtered_variance_final_event_time_idx ON bas.ble_tag_event_filtered_variance_final USING btree (event_time);


--
-- Name: ble_tag_event_filtered_variance_final_tag_id_idx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX ble_tag_event_filtered_variance_final_tag_id_idx ON bas.ble_tag_event_filtered_variance_final USING btree (tag_id);


--
-- Name: ble_tag_event_filtered_variance_tag_id_idx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX ble_tag_event_filtered_variance_tag_id_idx ON bas.ble_tag_event_filtered_variance_20min USING btree (tag_id);


--
-- Name: dd_event_time_idx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX dd_event_time_idx ON bas.ble_tag_event_filtered_variance_10min USING btree (event_time);


--
-- Name: dd_tag_id_idx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX dd_tag_id_idx ON bas.ble_tag_event_filtered_variance_10min USING btree (tag_id);


--
-- Name: feed_bin_weight_event+tank_id_indx; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX "feed_bin_weight_event+tank_id_indx" ON bas.feed_bin_weight_event USING btree (tank_id);


--
-- Name: idx_ble_tag_event_base_id; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX idx_ble_tag_event_base_id ON bas.ble_tag_event USING btree (base_id);


--
-- Name: idx_ble_tag_event_filtered_event_time; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX idx_ble_tag_event_filtered_event_time ON bas.ble_tag_event_filtered USING btree (event_time);


--
-- Name: idx_ble_tag_event_filtered_tag_id; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX idx_ble_tag_event_filtered_tag_id ON bas.ble_tag_event_filtered USING btree (tag_id);


--
-- Name: idx_ble_tag_event_tag_id; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX idx_ble_tag_event_tag_id ON bas.ble_tag_event USING btree (tag_id);


--
-- Name: idx_bovine_ble_tag_unique; Type: INDEX; Schema: bas; Owner: -
--

CREATE UNIQUE INDEX idx_bovine_ble_tag_unique ON bas.ble_bovine_tag USING btree (bovine_id, tag_id, event_time);


--
-- Name: idx_feed_bin_weight_event_event_time; Type: INDEX; Schema: bas; Owner: -
--

CREATE INDEX idx_feed_bin_weight_event_event_time ON bas.feed_bin_weight_event USING btree (event_time);


--
-- Name: beef_Report_event_time_idx; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX "beef_Report_event_time_idx" ON batch.beef_report USING btree (event_date);


--
-- Name: cdn_progeny_potential_genetics_lpi_idx; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX cdn_progeny_potential_genetics_lpi_idx ON batch.cdn_progeny_potential_genetics USING btree (lpi);


--
-- Name: dam_reg_idx; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX dam_reg_idx ON batch.cdn_progeny_potential_genetics USING btree (dam_full_reg_number);


--
-- Name: g; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX g ON batch.aggregate_site_data USING btree (hol_webservice_data);


--
-- Name: id_idx; Type: INDEX; Schema: batch; Owner: -
--

CREATE UNIQUE INDEX id_idx ON batch.bovinecurr_long USING btree (id);


--
-- Name: idx_day_ingredient_usage_result_date; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX idx_day_ingredient_usage_result_date ON batch.day_ingredient_usage_result USING btree (date);


--
-- Name: idx_day_ingredient_usage_result_feed_type_id; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX idx_day_ingredient_usage_result_feed_type_id ON batch.day_ingredient_usage_result USING btree (feed_type_id);


--
-- Name: idx_full_reg_number; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX idx_full_reg_number ON batch.holstein_canada_data USING btree (breed, country, sex, reg_no);


--
-- Name: index_extract_date; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX index_extract_date ON batch.holstein_canada_data USING btree (extract_date);


--
-- Name: index_reg_no; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX index_reg_no ON batch.holstein_canada_data USING btree (reg_no);


--
-- Name: milk2020_hoof_expr_idx; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX milk2020_hoof_expr_idx ON batch.milk2020_hoof USING btree (((data ->> 'reg'::text)));


--
-- Name: milk2020_hoof_expr_idx1; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX milk2020_hoof_expr_idx1 ON batch.milk2020_hoof USING btree (((data ->> 'test_date'::text)));


--
-- Name: percent_inbreeding_idx; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX percent_inbreeding_idx ON batch.cdn_progeny_potential_genetics USING btree (percent_inbreeding);

ALTER TABLE batch.cdn_progeny_potential_genetics CLUSTER ON percent_inbreeding_idx;


--
-- Name: sire_reg_number_idx; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX sire_reg_number_idx ON batch.cdn_progeny_potential_genetics USING btree (sire_full_reg_number);


--
-- Name: valacta_raw_expr_idx; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX valacta_raw_expr_idx ON batch.valacta_raw USING btree (((data ->> 'reg'::text)));


--
-- Name: valacta_raw_expr_idx1; Type: INDEX; Schema: batch; Owner: -
--

CREATE INDEX valacta_raw_expr_idx1 ON batch.valacta_raw USING btree (((data ->> 'test_date'::text)));


--
-- Name: BCS_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX "BCS_index" ON bovinemanagement.body_condition_score_event USING btree (id);


--
-- Name: active_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX active_index ON bovinemanagement.location USING btree (active);


--
-- Name: actual_breeding_time_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX actual_breeding_time_index ON bovinemanagement.breeding_event USING btree (actual_breeding_time);


--
-- Name: bovine_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX bovine_id ON bovinemanagement.sale_price USING btree (bovine_id);


--
-- Name: bovine_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX bovine_id_idx ON bovinemanagement.comment_custom USING btree (bovine_id);


--
-- Name: bovine_id_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX bovine_id_index ON bovinemanagement.location_event USING btree (bovine_id);


--
-- Name: bovine_id_index_location_sort; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX bovine_id_index_location_sort ON bovinemanagement.location_sort USING btree (bovine_id);


--
-- Name: breeding_event_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX breeding_event_id ON bovinemanagement.semen_straw USING btree (breeding_event_id);

ALTER TABLE bovinemanagement.semen_straw CLUSTER ON breeding_event_id;


--
-- Name: calf_event_idx_partial_Reg; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX "calf_event_idx_partial_Reg" ON bovinemanagement.calving_event USING btree (calf_rfid_number);


--
-- Name: calf_milk_administered_bovine_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX calf_milk_administered_bovine_id_idx ON bovinemanagement.calf_milk_administered USING btree (bovine_id);


--
-- Name: calf_milk_administered_event_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX calf_milk_administered_event_time_idx ON bovinemanagement.calf_milk_administered USING btree (event_time);


--
-- Name: calf_milk_administered_medical_case_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX calf_milk_administered_medical_case_id_idx ON bovinemanagement.calf_milk_administered USING btree (medical_case_id);


--
-- Name: calf_milk_administered_medicine_index_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX calf_milk_administered_medicine_index_idx ON bovinemanagement.calf_milk_administered USING btree (calf_milk_id);


--
-- Name: calf_milk_administered_scheduled_event_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX calf_milk_administered_scheduled_event_time_idx ON bovinemanagement.calf_milk_administered USING btree (scheduled_event_time);


--
-- Name: calf_milk_administered_scheduled_userid_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX calf_milk_administered_scheduled_userid_idx ON bovinemanagement.calf_milk_administered USING btree (scheduled_userid);


--
-- Name: calf_milk_administered_update_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX calf_milk_administered_update_time_idx ON bovinemanagement.calf_milk_administered USING btree (update_time);


--
-- Name: calf_milk_administered_userid_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX calf_milk_administered_userid_idx ON bovinemanagement.calf_milk_administered USING btree (userid);


--
-- Name: cull_event_id_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX cull_event_id_index ON bovinemanagement.cull_event USING btree (id);


--
-- Name: death_date_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX death_date_index ON bovinemanagement.bovine USING btree (death_date);


--
-- Name: dehorn_event_bovine_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX dehorn_event_bovine_id_idx ON bovinemanagement.dehorn_event USING btree (bovine_id);


--
-- Name: dehorn_event_event_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX dehorn_event_event_time_idx ON bovinemanagement.dehorn_event USING btree (event_time);


--
-- Name: dehorn_event_kamar_event_type_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX dehorn_event_kamar_event_type_idx ON bovinemanagement.dehorn_event USING btree (dehorn_event_type);


--
-- Name: dehorn_event_userid_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX dehorn_event_userid_idx ON bovinemanagement.dehorn_event USING btree (userid);


--
-- Name: diagnosis_id_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX diagnosis_id_index ON bovinemanagement.medical_diagnosis USING btree (diagnosis_type_id);


--
-- Name: dnatest_event_bovine_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX dnatest_event_bovine_id_idx ON bovinemanagement.dnatest_event USING btree (bovine_id);


--
-- Name: dnatest_event_event_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX dnatest_event_event_time_idx ON bovinemanagement.dnatest_event USING btree (event_time);


--
-- Name: donor_dam_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX donor_dam_index ON bovinemanagement.embryo_flush USING btree (donor_dam_full_reg_number);


--
-- Name: dry_date_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX dry_date_index ON bovinemanagement.lactation USING btree (dry_date);


--
-- Name: estimated_breeding_embryo_event_id 	; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX "estimated_breeding_embryo_event_id 	" ON bovinemanagement.preg_check_event USING btree (estimated_breeding_embryo_event_id);


--
-- Name: estrus_bovine_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX estrus_bovine_id_idx ON bovinemanagement.estrus_event USING btree (bovine_id);


--
-- Name: estrus_event_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX estrus_event_time_idx ON bovinemanagement.estrus_event USING btree (event_time);

ALTER TABLE bovinemanagement.estrus_event CLUSTER ON estrus_event_time_idx;


--
-- Name: estrus_userid; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX estrus_userid ON bovinemanagement.estrus_event USING btree (userid);


--
-- Name: event_time; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX event_time ON bovinemanagement.sale_price USING btree (event_time);


--
-- Name: event_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX event_time_idx ON bovinemanagement.comment_custom USING btree (event_time);


--
-- Name: event_time_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX event_time_index ON bovinemanagement.location_event USING btree (event_time);


--
-- Name: event_time_index2; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX event_time_index2 ON bovinemanagement.embryo_implant USING btree (event_time);


--
-- Name: flush_date_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX flush_date_index ON bovinemanagement.embryo_flush USING btree (flush_date);


--
-- Name: foot_event_bovine_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX foot_event_bovine_id ON bovinemanagement.foot_event USING btree (bovine_id);

ALTER TABLE bovinemanagement.foot_event CLUSTER ON foot_event_bovine_id;


--
-- Name: foot_event_event_time; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX foot_event_event_time ON bovinemanagement.foot_event USING btree (event_time);


--
-- Name: foot_event_foot_type_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX foot_event_foot_type_id ON bovinemanagement.foot_event USING btree (foot_type_id);


--
-- Name: foot_event_userid; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX foot_event_userid ON bovinemanagement.foot_event USING btree (userid);


--
-- Name: fresh_date_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX fresh_date_index ON bovinemanagement.lactation USING btree (fresh_date);

ALTER TABLE bovinemanagement.lactation CLUSTER ON fresh_date_index;


--
-- Name: idx2_bovine_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx2_bovine_id ON bovinemanagement.breeding_event USING btree (bovine_id);

ALTER TABLE bovinemanagement.breeding_event CLUSTER ON idx2_bovine_id;


--
-- Name: idx_bovine_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_bovine_id ON bovinemanagement.embryo_implant USING btree (bovine_id);


--
-- Name: idx_breedin_event_estimated_breeding_time; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_breedin_event_estimated_breeding_time ON bovinemanagement.breeding_event USING btree (estimated_breeding_time);


--
-- Name: idx_breeding_event_state_frozen_false; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_breeding_event_state_frozen_false ON bovinemanagement.breeding_event USING btree (state_frozen) WHERE (state_frozen = false);


--
-- Name: idx_breeding_event_state_frozen_true; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_breeding_event_state_frozen_true ON bovinemanagement.breeding_event USING btree (state_frozen) WHERE (state_frozen = true);


--
-- Name: idx_dam_full_reg_number; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_dam_full_reg_number ON bovinemanagement.bovine USING btree (dam_full_reg_number);


--
-- Name: idx_death_date; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_death_date ON bovinemanagement.bovine USING btree (death_date);


--
-- Name: idx_death_date_null; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_death_date_null ON bovinemanagement.bovine USING btree (death_date) WHERE (death_date IS NULL);


--
-- Name: idx_eartag_bovine_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_eartag_bovine_id ON bovinemanagement.eartag USING btree (bovine_id);


--
-- Name: idx_eartag_eartag; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_eartag_eartag ON bovinemanagement.eartag USING btree (event_time);


--
-- Name: idx_eartag_side_left; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_eartag_side_left ON bovinemanagement.eartag USING btree (side) WHERE (side = 'left'::bpchar);


--
-- Name: idx_eartag_side_right; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_eartag_side_right ON bovinemanagement.eartag USING btree (side) WHERE (side = 'right'::bpchar);


--
-- Name: idx_full_name; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_full_name ON bovinemanagement.bovine USING btree (full_name);


--
-- Name: idx_full_reg_number; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_full_reg_number ON bovinemanagement.bovine USING btree (full_reg_number);


--
-- Name: idx_implanter_userid; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_implanter_userid ON bovinemanagement.embryo_implant USING btree (implanter_userid);


--
-- Name: idx_lactation_bovine_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_lactation_bovine_id ON bovinemanagement.lactation USING btree (bovine_id);


--
-- Name: idx_lactation_dry_date; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_lactation_dry_date ON bovinemanagement.lactation USING btree (dry_date);


--
-- Name: idx_lactation_dry_date_null; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_lactation_dry_date_null ON bovinemanagement.lactation USING btree (dry_date) WHERE (dry_date IS NULL);


--
-- Name: idx_location_event_location_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_location_event_location_id ON bovinemanagement.location_event USING btree (location_id);


--
-- Name: idx_location_event_trnasaction_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_location_event_trnasaction_id ON bovinemanagement.location_event USING btree (transaction_id);


--
-- Name: idx_location_on_farm_true; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_location_on_farm_true ON bovinemanagement.location USING btree (on_farm) WHERE (on_farm = true);


--
-- Name: idx_medical_ketone_bovine_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_medical_ketone_bovine_id ON bovinemanagement.medical_ketone USING btree (bovine_id);

ALTER TABLE bovinemanagement.medical_ketone CLUSTER ON idx_medical_ketone_bovine_id;


--
-- Name: idx_medical_ketone_medical_case_id; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_medical_ketone_medical_case_id ON bovinemanagement.medical_ketone USING btree (medical_case_id);


--
-- Name: idx_recipient_full_reg_number; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_recipient_full_reg_number ON bovinemanagement.bovine USING btree (recipient_full_reg_number);


--
-- Name: idx_sire_full_reg_number; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_sire_full_reg_number ON bovinemanagement.bovine USING btree (sire_full_reg_number);


--
-- Name: idx_sire_semen_code_sexed_semen_false; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_sire_semen_code_sexed_semen_false ON bovinemanagement.sire_semen_code USING btree (sexed_semen) WHERE (sexed_semen = false);


--
-- Name: idx_userid; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX idx_userid ON bovinemanagement.embryo_implant USING btree (userid);


--
-- Name: indx_location_ccia_reportable; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX indx_location_ccia_reportable ON bovinemanagement.location USING btree (ccia_reportable);


--
-- Name: kamar_bovine_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX kamar_bovine_id_idx ON bovinemanagement.kamar_event USING btree (bovine_id);

ALTER TABLE bovinemanagement.kamar_event CLUSTER ON kamar_bovine_id_idx;


--
-- Name: kamar_event_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX kamar_event_time_idx ON bovinemanagement.kamar_event USING btree (event_time);


--
-- Name: kamar_event_type_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX kamar_event_type_idx ON bovinemanagement.kamar_event USING btree (kamar_event_type);


--
-- Name: kamar_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX kamar_id_idx ON bovinemanagement.kamar_event USING btree (id);


--
-- Name: kamar_userid_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX kamar_userid_idx ON bovinemanagement.kamar_event USING btree (userid);


--
-- Name: medical_case_bovine_id_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medical_case_bovine_id_index ON bovinemanagement.medical_case USING btree (bovine_id);


--
-- Name: medical_case_close_date_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medical_case_close_date_index ON bovinemanagement.medical_case USING btree (close_date);


--
-- Name: medical_case_close_userid_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medical_case_close_userid_index ON bovinemanagement.medical_case USING btree (close_userid);


--
-- Name: medical_case_diagnosis_id_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medical_case_diagnosis_id_index ON bovinemanagement.medical_case USING btree (diagnosis_id);


--
-- Name: medical_case_open_date_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medical_case_open_date_index ON bovinemanagement.medical_case USING btree (open_date);


--
-- Name: medical_case_open_userid_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medical_case_open_userid_index ON bovinemanagement.medical_case USING btree (open_userid);


--
-- Name: medicine_action_bovine_index_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_action_bovine_index_idx ON bovinemanagement.medical_action USING btree (bovine_id);


--
-- Name: medicine_action_event_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_action_event_time_idx ON bovinemanagement.medical_action USING btree (event_time);


--
-- Name: medicine_action_medical_case_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_action_medical_case_id_idx ON bovinemanagement.medical_action USING btree (medical_case_id);


--
-- Name: medicine_action_medicine_index_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_action_medicine_index_idx ON bovinemanagement.medical_action USING btree (medical_action_type_id);


--
-- Name: medicine_action_scheduled_event_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_action_scheduled_event_time_idx ON bovinemanagement.medical_action USING btree (scheduled_event_time);


--
-- Name: medicine_action_scheduled_userid_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_action_scheduled_userid_idx ON bovinemanagement.medical_action USING btree (scheduled_userid);


--
-- Name: medicine_action_update_time_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_action_update_time_idx ON bovinemanagement.medical_action USING btree (update_time);


--
-- Name: medicine_action_userid_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_action_userid_idx ON bovinemanagement.medical_action USING btree (userid);


--
-- Name: medicine_administered_bovine_index__index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_administered_bovine_index__index ON bovinemanagement.medicine_administered USING btree (bovine_id);


--
-- Name: medicine_administered_event_time_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_administered_event_time_index ON bovinemanagement.medicine_administered USING btree (event_time);


--
-- Name: medicine_administered_medical_case_id_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_administered_medical_case_id_index ON bovinemanagement.medicine_administered USING btree (medical_case_id);


--
-- Name: medicine_administered_medicine_index_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_administered_medicine_index_index ON bovinemanagement.medicine_administered USING btree (medicine_index);


--
-- Name: medicine_administered_scheduled_event_time_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_administered_scheduled_event_time_index ON bovinemanagement.medicine_administered USING btree (scheduled_event_time);


--
-- Name: medicine_administered_scheduled_update_time_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_administered_scheduled_update_time_index ON bovinemanagement.medicine_administered USING btree (update_time);

ALTER TABLE bovinemanagement.medicine_administered CLUSTER ON medicine_administered_scheduled_update_time_index;


--
-- Name: medicine_administered_scheduled_userid_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_administered_scheduled_userid_index ON bovinemanagement.medicine_administered USING btree (scheduled_userid);


--
-- Name: medicine_administered_userid_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX medicine_administered_userid_index ON bovinemanagement.medicine_administered USING btree (userid);


--
-- Name: milking_location_index 	; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX "milking_location_index 	" ON bovinemanagement.location USING btree (milking_location);


--
-- Name: name_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX name_index ON bovinemanagement.location USING btree (name);


--
-- Name: on_farm_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX on_farm_index ON bovinemanagement.location USING btree (on_farm);


--
-- Name: open_date_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX open_date_index ON bovinemanagement.medical_case USING btree (open_date);


--
-- Name: primary_key2; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX primary_key2 ON bovinemanagement.bovine USING btree (id);

ALTER TABLE bovinemanagement.bovine CLUSTER ON primary_key2;


--
-- Name: reserved_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX reserved_idx ON bovinemanagement.semen_straw USING btree (reserved);


--
-- Name: semen_code_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX semen_code_index ON bovinemanagement.sire_semen_code USING btree (semen_code);


--
-- Name: semen_straw_breed_event_id_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX semen_straw_breed_event_id_idx ON bovinemanagement.semen_straw USING btree (breeding_event_id) WHERE NULL::boolean;


--
-- Name: semen_straw_discarded_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX semen_straw_discarded_idx ON bovinemanagement.semen_straw USING btree (discarded);


--
-- Name: semen_straw_id_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX semen_straw_id_index ON bovinemanagement.breeding_event USING btree (semen_straw_id);


--
-- Name: sire_full_reg_number_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX sire_full_reg_number_index ON bovinemanagement.sire_semen_code USING btree (sire_full_reg_number);


--
-- Name: transaction_id_index; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX transaction_id_index ON bovinemanagement.semen_straw USING btree (transaction_id);


--
-- Name: type_idx; Type: INDEX; Schema: bovinemanagement; Owner: -
--

CREATE INDEX type_idx ON bovinemanagement.comment_custom USING btree (type);


--
-- Name: field_parameter_event_time_field_id_idx; Type: INDEX; Schema: cropping; Owner: -
--

CREATE UNIQUE INDEX field_parameter_event_time_field_id_idx ON cropping.field_parameter USING btree (field_id, event_time);

ALTER TABLE cropping.field_parameter CLUSTER ON field_parameter_event_time_field_id_idx;


--
-- Name: plant_tissue_event_datum_id_idx; Type: INDEX; Schema: cropping; Owner: -
--

CREATE INDEX plant_tissue_event_datum_id_idx ON cropping.plant_tissue_event USING btree (datum_id);


--
-- Name: plant_tissue_event_field_id_idx; Type: INDEX; Schema: cropping; Owner: -
--

CREATE INDEX plant_tissue_event_field_id_idx ON cropping.plant_tissue_event USING btree (field_id);


--
-- Name: plant_tissue_event_id_idx; Type: INDEX; Schema: cropping; Owner: -
--

CREATE UNIQUE INDEX plant_tissue_event_id_idx ON cropping.plant_tissue_event USING btree (id);


--
-- Name: seed_category_index; Type: INDEX; Schema: cropping; Owner: -
--

CREATE INDEX seed_category_index ON cropping.seed_category USING btree (id);


--
-- Name: soil_sample_datum_id; Type: INDEX; Schema: cropping; Owner: -
--

CREATE INDEX soil_sample_datum_id ON cropping.soil_sample_event USING btree (datum_id);


--
-- Name: soil_sample_field_id; Type: INDEX; Schema: cropping; Owner: -
--

CREATE INDEX soil_sample_field_id ON cropping.soil_sample_event USING btree (field_id);


--
-- Name: soil_sample_id; Type: INDEX; Schema: cropping; Owner: -
--

CREATE INDEX soil_sample_id ON cropping.soil_sample_event USING btree (id);


--
-- Name: idx_coverage_geom; Type: INDEX; Schema: gis; Owner: -
--

CREATE INDEX idx_coverage_geom ON gis.coverage USING btree (the_geom);


--
-- Name: idx_summary_geo_point; Type: INDEX; Schema: gis; Owner: -
--

CREATE INDEX idx_summary_geo_point ON gis.summary USING btree (geo_point);


--
-- Name: index_id; Type: INDEX; Schema: hr; Owner: -
--

CREATE UNIQUE INDEX index_id ON hr.time_sheet USING btree (id);


--
-- Name: page_parent_id_idx; Type: INDEX; Schema: intwebsite; Owner: -
--

CREATE INDEX page_parent_id_idx ON intwebsite.page USING btree (parent_id);


--
-- Name: page_security_page_id_idx; Type: INDEX; Schema: intwebsite; Owner: -
--

CREATE INDEX page_security_page_id_idx ON intwebsite.page_security USING btree (pageid);

ALTER TABLE intwebsite.page_security CLUSTER ON page_security_page_id_idx;


--
-- Name: active_index; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX active_index ON nutrition.recipe USING btree (active);


--
-- Name: date; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX date ON nutrition.recipe USING btree (date);


--
-- Name: date_index; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX date_index ON nutrition.mix_modify USING btree (date);


--
-- Name: dateq_index; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX dateq_index ON nutrition.feed_moisture USING btree (date);


--
-- Name: event_time_index; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX event_time_index ON nutrition.bag_moisture_test USING btree (event_time);


--
-- Name: feed_id_date; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE UNIQUE INDEX feed_id_date ON nutrition.feed_moisture USING btree (feed_id, date);


--
-- Name: feed_id_index; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX feed_id_index ON nutrition.feed_moisture USING btree (feed_id);


--
-- Name: feedcurr_id; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX feedcurr_id ON nutrition.recipe_item USING btree (feedcurr_id);

ALTER TABLE nutrition.recipe_item CLUSTER ON feedcurr_id;


--
-- Name: mix_id; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX mix_id ON nutrition.mix_modify USING btree (mix_id);


--
-- Name: nrc_recipe_param_status_index; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX nrc_recipe_param_status_index ON nutrition.nrc_recipe_param USING btree (status);


--
-- Name: nutrition_bag_event_time_idx; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX nutrition_bag_event_time_idx ON nutrition.bag USING btree (event_time);


--
-- Name: nutrition_bag_location_idx; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX nutrition_bag_location_idx ON nutrition.bag USING btree (location);


--
-- Name: nutrition_bag_slot_idx; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX nutrition_bag_slot_idx ON nutrition.bag USING btree (slot);


--
-- Name: nutrition_recipe_active_idx; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX nutrition_recipe_active_idx ON nutrition.recipe USING btree (active);


--
-- Name: recipe_id; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX recipe_id ON nutrition.recipe_item USING btree (recipe_id);


--
-- Name: slot_index; Type: INDEX; Schema: nutrition; Owner: -
--

CREATE INDEX slot_index ON nutrition.bag USING btree (slot);


--
-- Name: pic_bovine_bovine_id_idx; Type: INDEX; Schema: picture; Owner: -
--

CREATE INDEX pic_bovine_bovine_id_idx ON picture.bovine USING btree (bovine_id);


--
-- Name: pic_bovine_eveent_time_idx; Type: INDEX; Schema: picture; Owner: -
--

CREATE INDEX pic_bovine_eveent_time_idx ON picture.bovine USING btree (event_time);


--
-- Name: combined_schema_table; Type: INDEX; Schema: system; Owner: -
--

CREATE UNIQUE INDEX combined_schema_table ON system.update_log USING btree (schema_name, table_name);


--
-- Name: idx_update_log_last_update; Type: INDEX; Schema: system; Owner: -
--

CREATE INDEX idx_update_log_last_update ON system.update_log USING btree (last_update);


--
-- Name: cow insert_create_time; Type: TRIGGER; Schema: alpro; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON alpro.cow FOR EACH ROW EXECUTE PROCEDURE alpro.create_time_column();


--
-- Name: sync insert_create_time; Type: TRIGGER; Schema: alpro; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON alpro.sync FOR EACH ROW EXECUTE PROCEDURE alpro.create_time_column();


--
-- Name: sort_gate_log insert_create_time; Type: TRIGGER; Schema: alpro; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON alpro.sort_gate_log FOR EACH ROW EXECUTE PROCEDURE alpro.create_time_column();


--
-- Name: cow insert_update_time; Type: TRIGGER; Schema: alpro; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON alpro.cow FOR EACH ROW EXECUTE PROCEDURE alpro.update_time_column();


--
-- Name: sync insert_update_time; Type: TRIGGER; Schema: alpro; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON alpro.sync FOR EACH ROW EXECUTE PROCEDURE alpro.update_time_column();


--
-- Name: cow stamp_update_log; Type: TRIGGER; Schema: alpro; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON alpro.cow FOR EACH STATEMENT EXECUTE PROCEDURE alpro.stamp_update_log();


--
-- Name: sync stamp_update_log; Type: TRIGGER; Schema: alpro; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON alpro.sync FOR EACH STATEMENT EXECUTE PROCEDURE alpro.stamp_update_log();


--
-- Name: cow update_update_time; Type: TRIGGER; Schema: alpro; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON alpro.cow FOR EACH ROW EXECUTE PROCEDURE alpro.update_time_column();


--
-- Name: sync update_update_time; Type: TRIGGER; Schema: alpro; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON alpro.sync FOR EACH ROW EXECUTE PROCEDURE alpro.update_time_column();


--
-- Name: daily_number_cows_milking insert_create_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON batch.daily_number_cows_milking FOR EACH ROW EXECUTE PROCEDURE batch.create_time_column();


--
-- Name: cdn_progeny_potential_genetics insert_create_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON batch.cdn_progeny_potential_genetics FOR EACH ROW EXECUTE PROCEDURE batch.create_time_column();


--
-- Name: quota insert_create_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON batch.quota FOR EACH ROW EXECUTE PROCEDURE batch.create_time_column();


--
-- Name: error_curr insert_create_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON batch.error_curr FOR EACH ROW EXECUTE PROCEDURE batch.create_time_column();


--
-- Name: holstein_canada_registered insert_create_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON batch.holstein_canada_registered FOR EACH ROW EXECUTE PROCEDURE batch.create_time_column();


--
-- Name: credit insert_create_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON batch.credit FOR EACH ROW EXECUTE PROCEDURE batch.create_time_column();


--
-- Name: milk_pickup_event insert_create_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON batch.milk_pickup_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: incentive_day insert_create_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON batch.incentive_day FOR EACH ROW EXECUTE PROCEDURE batch.create_time_column();


--
-- Name: aggregate_site_data insert_create_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON batch.aggregate_site_data FOR EACH ROW EXECUTE PROCEDURE batch.create_time_column();


--
-- Name: daily_number_cows_milking insert_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON batch.daily_number_cows_milking FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: cdn_progeny_potential_genetics insert_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON batch.cdn_progeny_potential_genetics FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: quota insert_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON batch.quota FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: error_curr insert_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON batch.error_curr FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: holstein_canada_registered insert_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON batch.holstein_canada_registered FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: credit insert_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON batch.credit FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: milk_pickup_event insert_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON batch.milk_pickup_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: incentive_day insert_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON batch.incentive_day FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: aggregate_site_data insert_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON batch.aggregate_site_data FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: nb_bulk_tank_sample stamp_update_log; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON batch.nb_bulk_tank_sample FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: milk_pickup_event stamp_update_log; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON batch.milk_pickup_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: daily_number_cows_milking update_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON batch.daily_number_cows_milking FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: cdn_progeny_potential_genetics update_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON batch.cdn_progeny_potential_genetics FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: quota update_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON batch.quota FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: error_curr update_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON batch.error_curr FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: holstein_canada_registered update_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON batch.holstein_canada_registered FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: credit update_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON batch.credit FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: milk_pickup_event update_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON batch.milk_pickup_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: incentive_day update_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON batch.incentive_day FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: aggregate_site_data update_update_time; Type: TRIGGER; Schema: batch; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON batch.aggregate_site_data FOR EACH ROW EXECUTE PROCEDURE batch.update_time_column();


--
-- Name: dehorn_event create_dehorn_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_dehorn_create_time BEFORE INSERT ON bovinemanagement.dehorn_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: dehorn_event create_dehorn_create_time2; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_dehorn_create_time2 BEFORE INSERT ON bovinemanagement.dehorn_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: dnatest_event create_dnatest_event_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_dnatest_event_create_time BEFORE INSERT ON bovinemanagement.dnatest_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: dnatest_event create_dnatest_event_create_time2; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_dnatest_event_create_time2 BEFORE INSERT ON bovinemanagement.dnatest_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: eartag create_eartag_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_eartag_create_time BEFORE INSERT ON bovinemanagement.eartag FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: eartag create_eartag_create_time2; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_eartag_create_time2 BEFORE INSERT ON bovinemanagement.eartag FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: eartag_valid create_eartag_valid_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_eartag_valid_create_time BEFORE INSERT ON bovinemanagement.eartag_valid FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: eartag_valid create_eartag_valid_create_time2; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_eartag_valid_create_time2 BEFORE INSERT ON bovinemanagement.eartag_valid FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: estrus_event create_estrus_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_estrus_create_time BEFORE INSERT ON bovinemanagement.estrus_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: estrus_event create_estrus_create_time2; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER create_estrus_create_time2 BEFORE INSERT ON bovinemanagement.estrus_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: location_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.location_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: lactation insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.lactation FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: bovine insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.bovine FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: calving_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.calving_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: breeding_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.breeding_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: medicine_administered insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.medicine_administered FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: kamar_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.kamar_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: sire insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.sire FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: semen_straw insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.semen_straw FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: preg_check_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.preg_check_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: preg_check_type insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.preg_check_type FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: embryo_flush insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.embryo_flush FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: protocol_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.protocol_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: service_picks insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.service_picks FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: embryo_implant insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.embryo_implant FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: cull_list_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.cull_list_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: sale_price insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.sale_price FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: comment_custom insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.comment_custom FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: foot_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.foot_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: medical_comment insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.medical_comment FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: medical_case insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.medical_case FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: medical_temperature insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.medical_temperature FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: medical_diagnosis insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.medical_diagnosis FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: cull_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.cull_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: medical_ketone insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.medical_ketone FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: location_sort insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.location_sort FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: medical_magnet insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.medical_magnet FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: body_condition_score_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.body_condition_score_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: vet_to_check_event insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.vet_to_check_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: calf_potential_name insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.calf_potential_name FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: embryo_straw insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.embryo_straw FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: production_item insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.production_item FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: medical_action insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.medical_action FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: calf_milk_administered insert_create_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON bovinemanagement.calf_milk_administered FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: location_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.location_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: lactation insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.lactation FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: bovine insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.bovine FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: calving_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.calving_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: breeding_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.breeding_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medicine_administered insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.medicine_administered FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: kamar_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.kamar_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: sire insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.sire FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: semen_straw insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.semen_straw FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: preg_check_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.preg_check_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: preg_check_type insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.preg_check_type FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: embryo_flush insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.embryo_flush FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: protocol_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.protocol_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: service_picks insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.service_picks FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: embryo_implant insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.embryo_implant FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cull_list_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.cull_list_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: sale_price insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.sale_price FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: comment_custom insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.comment_custom FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: foot_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.foot_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_comment insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.medical_comment FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_case insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.medical_case FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_temperature insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.medical_temperature FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_diagnosis insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.medical_diagnosis FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cull_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.cull_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_ketone insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.medical_ketone FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: location_sort insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.location_sort FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_magnet insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.medical_magnet FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: body_condition_score_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.body_condition_score_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: vet_to_check_event insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.vet_to_check_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: calf_potential_name insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.calf_potential_name FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: embryo_straw insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.embryo_straw FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: production_item insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.production_item FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_action insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.medical_action FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: calf_milk_administered insert_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON bovinemanagement.calf_milk_administered FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: estrus_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.estrus_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: estrus_type stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.estrus_type FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: bovine stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.bovine FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: breeding_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.breeding_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: protocol_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.protocol_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: calving_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.calving_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: calving_event_ease_types stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.calving_event_ease_types FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: cull_list_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.cull_list_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: eartag stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.eartag FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: embryo_flush stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.embryo_flush FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: embryo_implant stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.embryo_implant FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: embryo_implant_comment stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.embryo_implant_comment FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: embryo_straw stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.embryo_straw FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: foot_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.foot_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: kamar_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.kamar_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: lactation stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.lactation FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: location stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.location FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: location_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.location_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: medicine stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.medicine FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: medicine_administered stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.medicine_administered FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: palpate_comment stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.palpate_comment FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: preg_check_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.preg_check_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: preg_check_type stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.preg_check_type FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: semen_straw stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.semen_straw FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: service_picks stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.service_picks FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: sire stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.sire FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: sire_semen_code stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.sire_semen_code FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: sale_price stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.sale_price FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: comment_custom stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.comment_custom FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: medical_comment stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.medical_comment FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: medical_case stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.medical_case FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: medical_temperature stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.medical_temperature FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: medical_diagnosis stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.medical_diagnosis FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: cull_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.cull_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: medical_ketone stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.medical_ketone FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: location_sort stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.location_sort FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: medical_magnet stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.medical_magnet FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: body_condition_score_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.body_condition_score_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: vet_to_check_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.vet_to_check_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: calf_potential_name stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.calf_potential_name FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: dnatest_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.dnatest_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: production_item stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.production_item FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: medical_action stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.medical_action FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: calf_milk_administered stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.calf_milk_administered FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: dehorn_event stamp_update_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.dehorn_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: eartag_valid stamp_update_valid_log; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER stamp_update_valid_log AFTER INSERT OR DELETE OR UPDATE ON bovinemanagement.eartag_valid FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: dehorn_event update_dehorn_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_dehorn_update_time BEFORE UPDATE ON bovinemanagement.dehorn_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: dnatest_event update_dnatest_event_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_dnatest_event_update_time BEFORE UPDATE ON bovinemanagement.dnatest_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: eartag update_eartag_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_eartag_update_time BEFORE UPDATE ON bovinemanagement.eartag FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: eartag_valid update_eartag_valid_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_eartag_valid_update_time BEFORE UPDATE ON bovinemanagement.eartag_valid FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: estrus_event update_estrus_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_estrus_update_time BEFORE UPDATE ON bovinemanagement.estrus_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: location_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.location_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: lactation update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.lactation FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: bovine update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.bovine FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: calving_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.calving_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: breeding_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.breeding_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medicine_administered update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.medicine_administered FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: kamar_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.kamar_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: sire update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.sire FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: semen_straw update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.semen_straw FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: preg_check_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.preg_check_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: preg_check_type update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.preg_check_type FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: embryo_flush update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.embryo_flush FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: protocol_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.protocol_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: service_picks update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.service_picks FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: embryo_implant update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.embryo_implant FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cull_list_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.cull_list_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: sale_price update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.sale_price FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: comment_custom update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.comment_custom FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: foot_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.foot_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_comment update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.medical_comment FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_case update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.medical_case FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_temperature update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.medical_temperature FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_diagnosis update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.medical_diagnosis FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cull_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.cull_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_ketone update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.medical_ketone FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: location_sort update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.location_sort FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_magnet update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.medical_magnet FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: body_condition_score_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.body_condition_score_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: vet_to_check_event update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.vet_to_check_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: calf_potential_name update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.calf_potential_name FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: embryo_straw update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.embryo_straw FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: production_item update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.production_item FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: medical_action update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.medical_action FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: calf_milk_administered update_update_time; Type: TRIGGER; Schema: bovinemanagement; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON bovinemanagement.calf_milk_administered FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: seed_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.seed_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: seed_inventory insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.seed_inventory FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: manure_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.manure_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: seed_event_scheduled insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.seed_event_scheduled FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: tillage_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.tillage_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: harvest_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.harvest_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: spray_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.spray_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: border_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.border_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: field_parameter insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.field_parameter FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: fertilizer_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.fertilizer_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: fertilizer_event_scheduled insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.fertilizer_event_scheduled FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: lime_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.lime_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: comment_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.comment_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: spray insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.spray FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: general_comment_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.general_comment_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: soil_sample_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.soil_sample_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: plant_tissue_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.plant_tissue_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: yield_event insert_create_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON cropping.yield_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: seed_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.seed_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: seed_inventory insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.seed_inventory FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: manure_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.manure_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: seed_event_scheduled insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.seed_event_scheduled FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: tillage_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.tillage_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: harvest_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.harvest_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: spray_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.spray_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: border_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.border_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: field_parameter insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.field_parameter FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: fertilizer_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.fertilizer_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: fertilizer_event_scheduled insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.fertilizer_event_scheduled FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: lime_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.lime_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: comment_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.comment_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: spray insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.spray FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: general_comment_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.general_comment_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: soil_sample_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.soil_sample_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: plant_tissue_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.plant_tissue_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: yield_event insert_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON cropping.yield_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: seed_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.seed_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: seed_inventory stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.seed_inventory FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: manure_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.manure_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: seed_event_scheduled stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.seed_event_scheduled FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: tillage_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.tillage_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: harvest_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.harvest_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: spray_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.spray_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: border_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.border_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: field_parameter stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.field_parameter FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: fertilizer_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.fertilizer_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: fertilizer_event_scheduled stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.fertilizer_event_scheduled FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: lime_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.lime_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: comment_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.comment_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: spray stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.spray FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: general_comment_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.general_comment_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: soil_sample_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.soil_sample_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: plant_tissue_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.plant_tissue_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: yield_event stamp_update_log; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON cropping.yield_event FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: seed_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.seed_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: seed_inventory update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.seed_inventory FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: manure_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.manure_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: seed_event_scheduled update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.seed_event_scheduled FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: tillage_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.tillage_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: harvest_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.harvest_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: spray_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.spray_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: border_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.border_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: field_parameter update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.field_parameter FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: fertilizer_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.fertilizer_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: fertilizer_event_scheduled update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.fertilizer_event_scheduled FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: lime_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.lime_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: comment_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.comment_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: spray update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.spray FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: general_comment_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.general_comment_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: soil_sample_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.soil_sample_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: plant_tissue_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.plant_tissue_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: yield_event update_update_time; Type: TRIGGER; Schema: cropping; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON cropping.yield_event FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: time_sheet insert_create_time; Type: TRIGGER; Schema: hr; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON hr.time_sheet FOR EACH ROW EXECUTE PROCEDURE hr.create_time_column();


--
-- Name: overtime insert_create_time; Type: TRIGGER; Schema: hr; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON hr.overtime FOR EACH ROW EXECUTE PROCEDURE hr.create_time_column();


--
-- Name: time_sheet insert_update_time; Type: TRIGGER; Schema: hr; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON hr.time_sheet FOR EACH ROW EXECUTE PROCEDURE hr.update_time_column();


--
-- Name: overtime insert_update_time; Type: TRIGGER; Schema: hr; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON hr.overtime FOR EACH ROW EXECUTE PROCEDURE hr.update_time_column();


--
-- Name: time_sheet stamp_update_log; Type: TRIGGER; Schema: hr; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON hr.time_sheet FOR EACH STATEMENT EXECUTE PROCEDURE hr.stamp_update_log();


--
-- Name: overtime stamp_update_log; Type: TRIGGER; Schema: hr; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON hr.overtime FOR EACH STATEMENT EXECUTE PROCEDURE hr.stamp_update_log();


--
-- Name: time_sheet update_update_time; Type: TRIGGER; Schema: hr; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON hr.time_sheet FOR EACH ROW EXECUTE PROCEDURE hr.update_time_column();


--
-- Name: overtime update_update_time; Type: TRIGGER; Schema: hr; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON hr.overtime FOR EACH ROW EXECUTE PROCEDURE hr.update_time_column();


--
-- Name: hours_log insert_create_time; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON machinery.hours_log FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: service_administered insert_create_time; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON machinery.service_administered FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: comment insert_create_time; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON machinery.comment FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: hours_log insert_update_time; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON machinery.hours_log FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: service_administered insert_update_time; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON machinery.service_administered FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: comment insert_update_time; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON machinery.comment FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: hours_log stamp_update_log; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON machinery.hours_log FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: service_administered stamp_update_log; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON machinery.service_administered FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: comment stamp_update_log; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON machinery.comment FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: hours_log update_update_time; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON machinery.hours_log FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: service_administered update_update_time; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON machinery.service_administered FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: comment update_update_time; Type: TRIGGER; Schema: machinery; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON machinery.comment FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: purchases insert_create_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON misc.purchases FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: task_completed insert_create_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON misc.task_completed FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: sop insert_create_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON misc.sop FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: sop_content insert_create_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON misc.sop_content FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: cqm_record17 insert_create_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON misc.cqm_record17 FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: cqm_record13 insert_create_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON misc.cqm_record13 FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: cqm_record9 insert_create_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON misc.cqm_record9 FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: chat_text insert_create_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON misc.chat_text FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.create_time_column();


--
-- Name: purchases insert_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON misc.purchases FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: task_completed insert_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON misc.task_completed FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: sop insert_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON misc.sop FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: sop_content insert_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON misc.sop_content FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cqm_record17 insert_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON misc.cqm_record17 FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cqm_record13 insert_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON misc.cqm_record13 FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cqm_record9 insert_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON misc.cqm_record9 FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: chat_text insert_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON misc.chat_text FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: purchases stamp_update_log; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON misc.purchases FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: task_completed stamp_update_log; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON misc.task_completed FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: sop stamp_update_log; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON misc.sop FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: sop_content stamp_update_log; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON misc.sop_content FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: cqm_record17 stamp_update_log; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON misc.cqm_record17 FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: cqm_record13 stamp_update_log; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON misc.cqm_record13 FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: cqm_record9 stamp_update_log; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON misc.cqm_record9 FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: chat_text stamp_update_log; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON misc.chat_text FOR EACH STATEMENT EXECUTE PROCEDURE bovinemanagement.stamp_update_log();


--
-- Name: purchases update_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON misc.purchases FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: task_completed update_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON misc.task_completed FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: sop update_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON misc.sop FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: sop_content update_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON misc.sop_content FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cqm_record17 update_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON misc.cqm_record17 FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cqm_record13 update_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON misc.cqm_record13 FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: cqm_record9 update_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON misc.cqm_record9 FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: chat_text update_update_time; Type: TRIGGER; Schema: misc; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON misc.chat_text FOR EACH ROW EXECUTE PROCEDURE bovinemanagement.update_time_column();


--
-- Name: bag insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.bag FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: bag_consumption insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.bag_consumption FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: bag_moisture_test insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.bag_moisture_test FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: bag_comment insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.bag_comment FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: bag_ensile_date insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.bag_ensile_date FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: bag_feed insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.bag_feed FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: bag_field insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.bag_field FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: recipe insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.recipe FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: recipe_item insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.recipe_item FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: bag_cost insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.bag_cost FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: nrc_recipe_param insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.nrc_recipe_param FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: nrc_recipe_item_feed_log insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.nrc_recipe_item_feed_log FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: bag_analysis insert_create_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_create_time BEFORE INSERT ON nutrition.bag_analysis FOR EACH ROW EXECUTE PROCEDURE nutrition.create_time_column();


--
-- Name: bag insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.bag FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_consumption insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.bag_consumption FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_moisture_test insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.bag_moisture_test FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_comment insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.bag_comment FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_ensile_date insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.bag_ensile_date FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_feed insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.bag_feed FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_field insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.bag_field FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: recipe insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.recipe FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: recipe_item insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.recipe_item FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_cost insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.bag_cost FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: nrc_recipe_param insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.nrc_recipe_param FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: nrc_recipe_item_feed_log insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.nrc_recipe_item_feed_log FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_analysis insert_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER insert_update_time BEFORE INSERT ON nutrition.bag_analysis FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.bag FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: bag_consumption stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.bag_consumption FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: bag_moisture_test stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.bag_moisture_test FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: bag_comment stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.bag_comment FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: bag_ensile_date stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.bag_ensile_date FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: bag_feed stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.bag_feed FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: bag_field stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.bag_field FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: recipe stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.recipe FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: recipe_item stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.recipe_item FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: bag_cost stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.bag_cost FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: nrc_recipe_param stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.nrc_recipe_param FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: nrc_recipe_item_feed_log stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.nrc_recipe_item_feed_log FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: bag_analysis stamp_update_log; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER stamp_update_log AFTER INSERT OR DELETE OR UPDATE ON nutrition.bag_analysis FOR EACH STATEMENT EXECUTE PROCEDURE nutrition.stamp_update_log();


--
-- Name: bag update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.bag FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_consumption update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.bag_consumption FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_moisture_test update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.bag_moisture_test FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_comment update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.bag_comment FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_ensile_date update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.bag_ensile_date FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_feed update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.bag_feed FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_field update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.bag_field FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: recipe update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.recipe FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: recipe_item update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.recipe_item FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_cost update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.bag_cost FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: nrc_recipe_param update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.nrc_recipe_param FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: nrc_recipe_item_feed_log update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.nrc_recipe_item_feed_log FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: bag_analysis update_update_time; Type: TRIGGER; Schema: nutrition; Owner: -
--

CREATE TRIGGER update_update_time BEFORE UPDATE ON nutrition.bag_analysis FOR EACH ROW EXECUTE PROCEDURE nutrition.update_time_column();


--
-- Name: ccia_reported ccia_reported_location_event_id_fkey; Type: FK CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.ccia_reported
    ADD CONSTRAINT ccia_reported_location_event_id_fkey FOREIGN KEY (location_event_id) REFERENCES bovinemanagement.location_event(id);


--
-- Name: credit_exchange credit_exchange_userid_fkey; Type: FK CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.credit_exchange
    ADD CONSTRAINT credit_exchange_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: credit credit_userid_fkey; Type: FK CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.credit
    ADD CONSTRAINT credit_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: employee_shift employee_shift_userid_fkey; Type: FK CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.employee_shift
    ADD CONSTRAINT employee_shift_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: incentive_day incentive_day_userid_fkey; Type: FK CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.incentive_day
    ADD CONSTRAINT incentive_day_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: milk_pickup_event milk_pickup_event_userid_fkey; Type: FK CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.milk_pickup_event
    ADD CONSTRAINT milk_pickup_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: milk_statement milk_statement_userid_fkey; Type: FK CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.milk_statement
    ADD CONSTRAINT milk_statement_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: quota quota_userid_fkey; Type: FK CONSTRAINT; Schema: batch; Owner: -
--

ALTER TABLE ONLY batch.quota
    ADD CONSTRAINT quota_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: body_condition_score_event body_condition_score_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.body_condition_score_event
    ADD CONSTRAINT body_condition_score_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: bovine bovine_ownerid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.bovine
    ADD CONSTRAINT bovine_ownerid_fkey FOREIGN KEY (ownerid) REFERENCES wcauthentication.owners(ownerid);


--
-- Name: breeding_event breeding_event_actual_breeding_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.breeding_event
    ADD CONSTRAINT breeding_event_actual_breeding_userid_fkey FOREIGN KEY (actual_breeding_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: breeding_event breeding_event_comment_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.breeding_event
    ADD CONSTRAINT breeding_event_comment_id_fkey FOREIGN KEY (comment_id) REFERENCES bovinemanagement.palpate_comment(id);


--
-- Name: breeding_event breeding_event_estimated_breeding_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.breeding_event
    ADD CONSTRAINT breeding_event_estimated_breeding_userid_fkey FOREIGN KEY (estimated_breeding_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: breeding_event breeding_event_semen_straw_choice_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.breeding_event
    ADD CONSTRAINT breeding_event_semen_straw_choice_userid_fkey FOREIGN KEY (semen_straw_choice_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: breeding_event breeding_event_semen_straw_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.breeding_event
    ADD CONSTRAINT breeding_event_semen_straw_id_fkey FOREIGN KEY (semen_straw_id) REFERENCES bovinemanagement.semen_straw(id);


--
-- Name: breeding_event breeding_event_uuid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.breeding_event
    ADD CONSTRAINT breeding_event_uuid_fkey FOREIGN KEY (protocol_uuid) REFERENCES bovinemanagement.protocol_event(protocol_uuid) ON DELETE CASCADE;


--
-- Name: protocol_event breeding_protocol_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.protocol_event
    ADD CONSTRAINT breeding_protocol_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: calf_milk_administered calf_milk_administered_calf_milk_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calf_milk_administered
    ADD CONSTRAINT calf_milk_administered_calf_milk_id_fkey FOREIGN KEY (calf_milk_id) REFERENCES bovinemanagement.calf_milk(id);


--
-- Name: calf_milk_administered calf_milk_administered_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calf_milk_administered
    ADD CONSTRAINT calf_milk_administered_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: calf_potential_name calf_potential_name_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calf_potential_name
    ADD CONSTRAINT calf_potential_name_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: calving_event calving_event_calving_ease_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event
    ADD CONSTRAINT calving_event_calving_ease_fkey FOREIGN KEY (calving_ease) REFERENCES bovinemanagement.calving_event_ease_types(id);


--
-- Name: calving_event calving_event_lactation_id_fkey	; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event
    ADD CONSTRAINT "calving_event_lactation_id_fkey	" FOREIGN KEY (lactation_id) REFERENCES bovinemanagement.lactation(id) ON DELETE CASCADE;


--
-- Name: calving_event calving_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.calving_event
    ADD CONSTRAINT calving_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: comment_custom comment_custom_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.comment_custom
    ADD CONSTRAINT comment_custom_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: cull_event cull_event_location_event_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.cull_event
    ADD CONSTRAINT cull_event_location_event_id_fkey FOREIGN KEY (location_event_id) REFERENCES bovinemanagement.location_event(id);


--
-- Name: cull_event cull_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.cull_event
    ADD CONSTRAINT cull_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: cull_list_event cull_list_event_bovine_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.cull_list_event
    ADD CONSTRAINT cull_list_event_bovine_id_fkey FOREIGN KEY (bovine_id) REFERENCES bovinemanagement.bovine(id);


--
-- Name: cull_list_event cull_list_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.cull_list_event
    ADD CONSTRAINT cull_list_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: dnatest_event dnatest_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dnatest_event
    ADD CONSTRAINT dnatest_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: dryoff_event dryoff_event_lactation_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_lactation_id_fkey FOREIGN KEY (lactation_id) REFERENCES bovinemanagement.lactation(id);


--
-- Name: dryoff_event dryoff_event_med_administered_a1_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_med_administered_a1_fkey FOREIGN KEY (med_administered_a1) REFERENCES bovinemanagement.medicine_administered(id);


--
-- Name: dryoff_event dryoff_event_med_administered_a2_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_med_administered_a2_fkey FOREIGN KEY (med_administered_a2) REFERENCES bovinemanagement.medicine_administered(id);


--
-- Name: dryoff_event dryoff_event_med_administered_a3_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_med_administered_a3_fkey FOREIGN KEY (med_administered_a3) REFERENCES bovinemanagement.medicine_administered(id);


--
-- Name: dryoff_event dryoff_event_med_administered_a4_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_med_administered_a4_fkey FOREIGN KEY (med_administered_a4) REFERENCES bovinemanagement.medicine_administered(id);


--
-- Name: dryoff_event dryoff_event_med_administered_b1_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_med_administered_b1_fkey FOREIGN KEY (med_administered_b1) REFERENCES bovinemanagement.medicine_administered(id);


--
-- Name: dryoff_event dryoff_event_med_administered_b2_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_med_administered_b2_fkey FOREIGN KEY (med_administered_b2) REFERENCES bovinemanagement.medicine_administered(id);


--
-- Name: dryoff_event dryoff_event_med_administered_b3_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_med_administered_b3_fkey FOREIGN KEY (med_administered_b3) REFERENCES bovinemanagement.medicine_administered(id);


--
-- Name: dryoff_event dryoff_event_med_administered_b4_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_med_administered_b4_fkey FOREIGN KEY (med_administered_b4) REFERENCES bovinemanagement.medicine_administered(id);


--
-- Name: dryoff_event dryoff_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.dryoff_event
    ADD CONSTRAINT dryoff_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: eartag eartag_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.eartag
    ADD CONSTRAINT eartag_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: embryo_flush embryo_flush_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_flush
    ADD CONSTRAINT embryo_flush_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: embryo_flush embryo_flush_uuid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_flush
    ADD CONSTRAINT embryo_flush_uuid_fkey FOREIGN KEY (protocol_uuid) REFERENCES bovinemanagement.protocol_event(protocol_uuid) ON DELETE CASCADE;


--
-- Name: embryo_implant embryo_implant_comment_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant
    ADD CONSTRAINT embryo_implant_comment_id_fkey FOREIGN KEY (comment_id) REFERENCES bovinemanagement.embryo_implant_comment(id);


--
-- Name: embryo_implant embryo_recipient_embryo_straw_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant
    ADD CONSTRAINT embryo_recipient_embryo_straw_id_fkey FOREIGN KEY (embryo_straw_id) REFERENCES bovinemanagement.embryo_straw(id);


--
-- Name: embryo_implant embryo_recipient_implanter_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant
    ADD CONSTRAINT embryo_recipient_implanter_userid_fkey FOREIGN KEY (implanter_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: embryo_implant embryo_recipient_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_implant
    ADD CONSTRAINT embryo_recipient_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: embryo_straw embryo_straw_embryo_flush_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.embryo_straw
    ADD CONSTRAINT embryo_straw_embryo_flush_id_fkey FOREIGN KEY (embryo_flush_id) REFERENCES bovinemanagement.embryo_flush(id);


--
-- Name: estrus_event estrus_estrus_event_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_event
    ADD CONSTRAINT estrus_estrus_event_id_fkey FOREIGN KEY (estrus_type_id) REFERENCES bovinemanagement.estrus_type(id);


--
-- Name: estrus_event estrus_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_event
    ADD CONSTRAINT estrus_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: estrus_event estrus_kamar_event_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.estrus_event
    ADD CONSTRAINT estrus_kamar_event_id_fkey FOREIGN KEY (kamar_event_id) REFERENCES bovinemanagement.kamar_event(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: foot_event foot_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.foot_event
    ADD CONSTRAINT foot_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: kamar_event kamar_event_estrus_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.kamar_event
    ADD CONSTRAINT kamar_event_estrus_id_fkey FOREIGN KEY (estrus_id) REFERENCES bovinemanagement.estrus_event(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: kamar_event kamar_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.kamar_event
    ADD CONSTRAINT kamar_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: lactation lactation_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.lactation
    ADD CONSTRAINT lactation_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: location_event location_event_bovine_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_event
    ADD CONSTRAINT location_event_bovine_id_fkey FOREIGN KEY (bovine_id) REFERENCES bovinemanagement.bovine(id) ON DELETE CASCADE;


--
-- Name: location_event location_event_location_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_event
    ADD CONSTRAINT location_event_location_id_fkey FOREIGN KEY (location_id) REFERENCES bovinemanagement.location(id);


--
-- Name: location_event location_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_event
    ADD CONSTRAINT location_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: location_sort location_sort_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_sort
    ADD CONSTRAINT location_sort_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: location_urban_feedstall location_urban_feedstall_location_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.location_urban_feedstall
    ADD CONSTRAINT location_urban_feedstall_location_id_fkey FOREIGN KEY (location_id) REFERENCES bovinemanagement.location(id);


--
-- Name: medical_action medical_action_protocol_uuid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_action
    ADD CONSTRAINT medical_action_protocol_uuid_fkey FOREIGN KEY (protocol_uuid) REFERENCES bovinemanagement.protocol_event(protocol_uuid) ON DELETE CASCADE;


--
-- Name: medical_action medical_action_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_action
    ADD CONSTRAINT medical_action_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: medical_action medical_action_uuid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_action
    ADD CONSTRAINT medical_action_uuid_fkey FOREIGN KEY (protocol_uuid) REFERENCES bovinemanagement.protocol_event(protocol_uuid) ON DELETE CASCADE;


--
-- Name: medical_case medical_case_close_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_case
    ADD CONSTRAINT medical_case_close_userid_fkey FOREIGN KEY (close_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: medical_case medical_case_diagnosis_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_case
    ADD CONSTRAINT medical_case_diagnosis_id_fkey FOREIGN KEY (diagnosis_id) REFERENCES bovinemanagement.medical_diagnosis_type(id);


--
-- Name: medical_case medical_case_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_case
    ADD CONSTRAINT medical_case_userid_fkey FOREIGN KEY (open_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: medical_comment medical_comment_medical_case_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_comment
    ADD CONSTRAINT medical_comment_medical_case_id_fkey FOREIGN KEY (medical_case_id) REFERENCES bovinemanagement.medical_case(id) ON DELETE CASCADE;


--
-- Name: medical_comment medical_comment_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_comment
    ADD CONSTRAINT medical_comment_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: medical_diagnosis medical_diagnosis_diagnosis_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_diagnosis
    ADD CONSTRAINT medical_diagnosis_diagnosis_id_fkey FOREIGN KEY (diagnosis_type_id) REFERENCES bovinemanagement.medical_diagnosis_type(id);


--
-- Name: medical_diagnosis medical_diagnosis_medical_case_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_diagnosis
    ADD CONSTRAINT medical_diagnosis_medical_case_id_fkey FOREIGN KEY (medical_case_id) REFERENCES bovinemanagement.medical_case(id) ON DELETE CASCADE;


--
-- Name: medical_diagnosis medical_diagnosis_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_diagnosis
    ADD CONSTRAINT medical_diagnosis_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: medical_ketone medical_ketone_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_ketone
    ADD CONSTRAINT medical_ketone_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: medical_magnet medical_magnet_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_magnet
    ADD CONSTRAINT medical_magnet_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: medical_temperature medical_temperature_medical_case_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_temperature
    ADD CONSTRAINT medical_temperature_medical_case_id_fkey FOREIGN KEY (medical_case_id) REFERENCES bovinemanagement.medical_case(id) ON DELETE CASCADE;


--
-- Name: medical_temperature medical_temperature_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medical_temperature
    ADD CONSTRAINT medical_temperature_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: medicine_administered medicine_administered_medical_case_id_fkey1; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medicine_administered
    ADD CONSTRAINT medicine_administered_medical_case_id_fkey1 FOREIGN KEY (medical_case_id) REFERENCES bovinemanagement.medical_case(id) ON DELETE CASCADE;


--
-- Name: medicine_administered medicine_administered_medicine_index_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medicine_administered
    ADD CONSTRAINT medicine_administered_medicine_index_fkey FOREIGN KEY (medicine_index) REFERENCES bovinemanagement.medicine(id);


--
-- Name: medicine_administered medicine_administered_protocol_uuid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medicine_administered
    ADD CONSTRAINT medicine_administered_protocol_uuid_fkey FOREIGN KEY (protocol_uuid) REFERENCES bovinemanagement.protocol_event(protocol_uuid);


--
-- Name: medicine_administered medicine_administered_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.medicine_administered
    ADD CONSTRAINT medicine_administered_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: preg_check_event preg_check_event_preg_check_comment_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_event
    ADD CONSTRAINT preg_check_event_preg_check_comment_id_fkey FOREIGN KEY (preg_check_comment_id) REFERENCES bovinemanagement.preg_check_comment(id);


--
-- Name: preg_check_event preg_check_event_preg_check_type_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_event
    ADD CONSTRAINT preg_check_event_preg_check_type_id_fkey FOREIGN KEY (preg_check_type_id) REFERENCES bovinemanagement.preg_check_type(id);


--
-- Name: preg_check_event preg_check_event_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.preg_check_event
    ADD CONSTRAINT preg_check_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: production_item production_item_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.production_item
    ADD CONSTRAINT production_item_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: sale_price sale_price_comment_id_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sale_price
    ADD CONSTRAINT sale_price_comment_id_fkey FOREIGN KEY (comment_id) REFERENCES bovinemanagement.sale_price_comment(id);


--
-- Name: sale_price sale_price_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sale_price
    ADD CONSTRAINT sale_price_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: semen_straw semen_straw_ownerid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.semen_straw
    ADD CONSTRAINT semen_straw_ownerid_fkey FOREIGN KEY (ownerid) REFERENCES wcauthentication.owners(ownerid);


--
-- Name: semen_straw semen_straw_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.semen_straw
    ADD CONSTRAINT semen_straw_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: service_picks service_picks_primary_one_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.service_picks
    ADD CONSTRAINT service_picks_primary_one_fkey FOREIGN KEY (primary_one) REFERENCES bovinemanagement.sire_semen_code(semen_code);


--
-- Name: service_picks service_picks_primary_three_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.service_picks
    ADD CONSTRAINT service_picks_primary_three_fkey FOREIGN KEY (primary_three) REFERENCES bovinemanagement.sire_semen_code(semen_code);


--
-- Name: service_picks service_picks_primary_two_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.service_picks
    ADD CONSTRAINT service_picks_primary_two_fkey FOREIGN KEY (primary_two) REFERENCES bovinemanagement.sire_semen_code(semen_code);


--
-- Name: service_picks service_picks_secondary_one_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.service_picks
    ADD CONSTRAINT service_picks_secondary_one_fkey FOREIGN KEY (secondary_one) REFERENCES bovinemanagement.sire_semen_code(semen_code);


--
-- Name: service_picks service_picks_secondary_three_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.service_picks
    ADD CONSTRAINT service_picks_secondary_three_fkey FOREIGN KEY (secondary_three) REFERENCES bovinemanagement.sire_semen_code(semen_code);


--
-- Name: service_picks service_picks_secondary_two_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.service_picks
    ADD CONSTRAINT service_picks_secondary_two_fkey FOREIGN KEY (secondary_two) REFERENCES bovinemanagement.sire_semen_code(semen_code);


--
-- Name: service_picks service_picks_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.service_picks
    ADD CONSTRAINT service_picks_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: sire_semen_code sire_semen_code_sire_full_reg_number_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sire_semen_code
    ADD CONSTRAINT sire_semen_code_sire_full_reg_number_fkey FOREIGN KEY (sire_full_reg_number) REFERENCES bovinemanagement.sire(full_reg_number) MATCH FULL ON DELETE CASCADE;


--
-- Name: sire sire_userid_fkey; Type: FK CONSTRAINT; Schema: bovinemanagement; Owner: -
--

ALTER TABLE ONLY bovinemanagement.sire
    ADD CONSTRAINT sire_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: border_event border_event_datum_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.border_event
    ADD CONSTRAINT border_event_datum_id_fkey FOREIGN KEY (datum_id) REFERENCES cropping.datum(id);


--
-- Name: border_event border_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.border_event
    ADD CONSTRAINT border_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: border_event border_field_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.border_event
    ADD CONSTRAINT border_field_id_fkey FOREIGN KEY (field_id) REFERENCES cropping.field(id);


--
-- Name: comment_event comment_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.comment_event
    ADD CONSTRAINT comment_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: fertilizer_event_scheduled fertilizer_event_scheduled_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.fertilizer_event_scheduled
    ADD CONSTRAINT fertilizer_event_scheduled_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: fertilizer_event fertilizer_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.fertilizer_event
    ADD CONSTRAINT fertilizer_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: field_parameter field_parameter_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.field_parameter
    ADD CONSTRAINT field_parameter_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: field_parameter field_parameters_field_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.field_parameter
    ADD CONSTRAINT field_parameters_field_id_fkey FOREIGN KEY (field_id) REFERENCES cropping.field(id);


--
-- Name: general_comment_event general_comment_event_datum_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.general_comment_event
    ADD CONSTRAINT general_comment_event_datum_id_fkey FOREIGN KEY (datum_id) REFERENCES cropping.datum(id);


--
-- Name: general_comment_event general_comment_event_field_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.general_comment_event
    ADD CONSTRAINT general_comment_event_field_id_fkey FOREIGN KEY (field_id) REFERENCES cropping.field(id);


--
-- Name: general_comment_event general_comment_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.general_comment_event
    ADD CONSTRAINT general_comment_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: harvest_event harvest_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.harvest_event
    ADD CONSTRAINT harvest_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: lime_event lime_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.lime_event
    ADD CONSTRAINT lime_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: manure_event manure_event_datum_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.manure_event
    ADD CONSTRAINT manure_event_datum_id_fkey FOREIGN KEY (datum_id) REFERENCES cropping.datum(id);


--
-- Name: manure_event manure_event_field_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.manure_event
    ADD CONSTRAINT manure_event_field_id_fkey FOREIGN KEY (field_id) REFERENCES cropping.field(id);


--
-- Name: manure_event manure_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.manure_event
    ADD CONSTRAINT manure_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: plant_tissue_event plant_tissue_event_field_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.plant_tissue_event
    ADD CONSTRAINT plant_tissue_event_field_id_fkey FOREIGN KEY (field_id) REFERENCES cropping.field(id);


--
-- Name: plant_tissue_event plant_tissue_event_plant_tissue_parameter_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.plant_tissue_event
    ADD CONSTRAINT plant_tissue_event_plant_tissue_parameter_id_fkey FOREIGN KEY (plant_tissue_parameter_id) REFERENCES cropping.plant_tissue_parameter(id);


--
-- Name: plant_tissue_event plant_tissue_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.plant_tissue_event
    ADD CONSTRAINT plant_tissue_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: seed_event seed_event_datum_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed_event
    ADD CONSTRAINT seed_event_datum_id_fkey FOREIGN KEY (datum_id) REFERENCES cropping.datum(id);


--
-- Name: seed_event seed_event_field_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed_event
    ADD CONSTRAINT seed_event_field_id_fkey FOREIGN KEY (field_id) REFERENCES cropping.field(id);


--
-- Name: seed_event_scheduled seed_event_scheduled_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed_event_scheduled
    ADD CONSTRAINT seed_event_scheduled_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: seed_event seed_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed_event
    ADD CONSTRAINT seed_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: seed_inventory seed_inventory_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.seed_inventory
    ADD CONSTRAINT seed_inventory_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: soil_sample_event soil_sample_event_field_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.soil_sample_event
    ADD CONSTRAINT soil_sample_event_field_id_fkey FOREIGN KEY (field_id) REFERENCES cropping.field(id);


--
-- Name: soil_sample_event soil_sample_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.soil_sample_event
    ADD CONSTRAINT soil_sample_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: spray_event spray_event_spay_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.spray_event
    ADD CONSTRAINT spray_event_spay_id_fkey FOREIGN KEY (spray_id) REFERENCES cropping.spray(id);


--
-- Name: spray_event spray_event_sprayer_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.spray_event
    ADD CONSTRAINT spray_event_sprayer_userid_fkey FOREIGN KEY (sprayer_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: spray_event spray_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.spray_event
    ADD CONSTRAINT spray_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: tillage_event tillage_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.tillage_event
    ADD CONSTRAINT tillage_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: yield_event yield_event_field_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.yield_event
    ADD CONSTRAINT yield_event_field_id_fkey FOREIGN KEY (field_id) REFERENCES cropping.field(id);


--
-- Name: yield_event yield_event_userid_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.yield_event
    ADD CONSTRAINT yield_event_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: yield_event yield_event_yield_type_id_fkey; Type: FK CONSTRAINT; Schema: cropping; Owner: -
--

ALTER TABLE ONLY cropping.yield_event
    ADD CONSTRAINT yield_event_yield_type_id_fkey FOREIGN KEY (yield_type_id) REFERENCES cropping.yield_type(id);


--
-- Name: overtime overtime_overtime_userid_fkey; Type: FK CONSTRAINT; Schema: hr; Owner: -
--

ALTER TABLE ONLY hr.overtime
    ADD CONSTRAINT overtime_overtime_userid_fkey FOREIGN KEY (overtime_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: overtime overtime_userid_fkey; Type: FK CONSTRAINT; Schema: hr; Owner: -
--

ALTER TABLE ONLY hr.overtime
    ADD CONSTRAINT overtime_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: time_sheet time_bank_userid_fkey; Type: FK CONSTRAINT; Schema: hr; Owner: -
--

ALTER TABLE ONLY hr.time_sheet
    ADD CONSTRAINT time_bank_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: ajax_security ajax_security_ajax_id_fkey; Type: FK CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.ajax_security
    ADD CONSTRAINT ajax_security_ajax_id_fkey FOREIGN KEY (ajax_id) REFERENCES intwebsite.ajax(id);


--
-- Name: ajax_security ajax_security_group_fkey; Type: FK CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.ajax_security
    ADD CONSTRAINT ajax_security_group_fkey FOREIGN KEY ("group") REFERENCES wcauthentication.groups(name);


--
-- Name: page_security page_security_group_fkey; Type: FK CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.page_security
    ADD CONSTRAINT page_security_group_fkey FOREIGN KEY ("group") REFERENCES wcauthentication.groups(name);


--
-- Name: page_security page_security_pageid_fkey; Type: FK CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.page_security
    ADD CONSTRAINT page_security_pageid_fkey FOREIGN KEY (pageid) REFERENCES intwebsite.page(pageid) ON DELETE CASCADE;


--
-- Name: sse_security sse_security_group_fkey; Type: FK CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.sse_security
    ADD CONSTRAINT sse_security_group_fkey FOREIGN KEY ("group") REFERENCES wcauthentication.groups(name);


--
-- Name: sse_security sse_security_sse_id_fkey; Type: FK CONSTRAINT; Schema: intwebsite; Owner: -
--

ALTER TABLE ONLY intwebsite.sse_security
    ADD CONSTRAINT sse_security_sse_id_fkey FOREIGN KEY (sse_id) REFERENCES intwebsite.sse(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: comment comment_userid_fkey; Type: FK CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.comment
    ADD CONSTRAINT comment_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: hours_log hours_log_machine_id_fkey; Type: FK CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.hours_log
    ADD CONSTRAINT hours_log_machine_id_fkey FOREIGN KEY (machine_id) REFERENCES machinery.machine(id);


--
-- Name: hours_log hours_log_userid_fkey; Type: FK CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.hours_log
    ADD CONSTRAINT hours_log_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: service_administered service_administered_hours_log_id_fkey; Type: FK CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.service_administered
    ADD CONSTRAINT service_administered_hours_log_id_fkey FOREIGN KEY (hours_log_id) REFERENCES machinery.hours_log(id);


--
-- Name: service_administered service_administered_service_item_id_fkey; Type: FK CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.service_administered
    ADD CONSTRAINT service_administered_service_item_id_fkey FOREIGN KEY (service_item_id) REFERENCES machinery.service_item(id);


--
-- Name: service_administered service_administered_userid_fkey; Type: FK CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.service_administered
    ADD CONSTRAINT service_administered_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: service_item service_item_machine_id_fkey; Type: FK CONSTRAINT; Schema: machinery; Owner: -
--

ALTER TABLE ONLY machinery.service_item
    ADD CONSTRAINT service_item_machine_id_fkey FOREIGN KEY (machine_id) REFERENCES machinery.machine(id);


--
-- Name: chat_text chat_text_foreign_userid; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.chat_text
    ADD CONSTRAINT chat_text_foreign_userid FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: cqm_record13 cqm_record13_userid_fkey; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.cqm_record13
    ADD CONSTRAINT cqm_record13_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: cqm_record17 cqm_record17_userid_fkey; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.cqm_record17
    ADD CONSTRAINT cqm_record17_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: cqm_record9 cqm_record9_userid_fkey; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.cqm_record9
    ADD CONSTRAINT cqm_record9_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: purchases purchases_purchasing_userid_fkey; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.purchases
    ADD CONSTRAINT purchases_purchasing_userid_fkey FOREIGN KEY (purchasing_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: purchases purchases_requesting_userid_fkey; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.purchases
    ADD CONSTRAINT purchases_requesting_userid_fkey FOREIGN KEY (requesting_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: sop_content sop_content_sop_id_fkey; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.sop_content
    ADD CONSTRAINT sop_content_sop_id_fkey FOREIGN KEY (sop_id) REFERENCES misc.sop(id) ON DELETE CASCADE;


--
-- Name: sop_content sop_content_userid_fkey; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.sop_content
    ADD CONSTRAINT sop_content_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: sop sop_userid_fkey; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.sop
    ADD CONSTRAINT sop_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: task_completed task_completed_userid_fkey; Type: FK CONSTRAINT; Schema: misc; Owner: -
--

ALTER TABLE ONLY misc.task_completed
    ADD CONSTRAINT task_completed_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: bag_comment bag_comment_bag_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_comment
    ADD CONSTRAINT bag_comment_bag_id_fkey FOREIGN KEY (bag_id) REFERENCES nutrition.bag(id);


--
-- Name: bag_comment bag_comment_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_comment
    ADD CONSTRAINT bag_comment_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: bag_consumption bag_consumption_bag_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_consumption
    ADD CONSTRAINT bag_consumption_bag_id_fkey FOREIGN KEY (bag_id) REFERENCES nutrition.bag(id);


--
-- Name: bag_consumption bag_consumption_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_consumption
    ADD CONSTRAINT bag_consumption_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: bag_cost bag_cost_bag_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_cost
    ADD CONSTRAINT bag_cost_bag_id_fkey FOREIGN KEY (bag_id) REFERENCES nutrition.bag(id);


--
-- Name: bag_cost bag_cost_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_cost
    ADD CONSTRAINT bag_cost_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: bag_ensile_date bag_ensile_date_bag_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_ensile_date
    ADD CONSTRAINT bag_ensile_date_bag_id_fkey FOREIGN KEY (bag_id) REFERENCES nutrition.bag(id);


--
-- Name: bag_ensile_date bag_ensile_date_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_ensile_date
    ADD CONSTRAINT bag_ensile_date_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: bag_feed bag_feed_bag_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_feed
    ADD CONSTRAINT bag_feed_bag_id_fkey FOREIGN KEY (bag_id) REFERENCES nutrition.bag(id);


--
-- Name: bag_feed bag_feed_feed_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_feed
    ADD CONSTRAINT bag_feed_feed_id_fkey FOREIGN KEY (feed_id) REFERENCES nutrition.feed_type(id);


--
-- Name: bag_feed bag_feed_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_feed
    ADD CONSTRAINT bag_feed_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: bag_field bag_field_bag_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_field
    ADD CONSTRAINT bag_field_bag_id_fkey FOREIGN KEY (bag_id) REFERENCES nutrition.bag(id);


--
-- Name: bag_field bag_field_field_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_field
    ADD CONSTRAINT bag_field_field_id_fkey FOREIGN KEY (field_id) REFERENCES cropping.field(id);


--
-- Name: bag_field bag_field_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_field
    ADD CONSTRAINT bag_field_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: bag_moisture_test bag_moisture_test_bag_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_moisture_test
    ADD CONSTRAINT bag_moisture_test_bag_id_fkey FOREIGN KEY (bag_id) REFERENCES nutrition.bag(id);


--
-- Name: bag_moisture_test bag_moisture_test_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag_moisture_test
    ADD CONSTRAINT bag_moisture_test_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: bag bag_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.bag
    ADD CONSTRAINT bag_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: feed_cost feed_cost_feed_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_cost
    ADD CONSTRAINT feed_cost_feed_id_fkey FOREIGN KEY (feed_id) REFERENCES nutrition.feed(id) ON DELETE CASCADE;


--
-- Name: feed_cost feed_cost_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_cost
    ADD CONSTRAINT feed_cost_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: feed_moisture feed_moisture_feed_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.feed_moisture
    ADD CONSTRAINT feed_moisture_feed_id_fkey FOREIGN KEY (feed_id) REFERENCES nutrition.feed(id) ON DELETE CASCADE;


--
-- Name: mix mix_location_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.mix
    ADD CONSTRAINT mix_location_id_fkey FOREIGN KEY (location_id) REFERENCES bovinemanagement.location(id);


--
-- Name: mix_modify mix_modify_mix_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.mix_modify
    ADD CONSTRAINT mix_modify_mix_id_fkey FOREIGN KEY (mix_id) REFERENCES nutrition.mix(id) ON DELETE CASCADE;


--
-- Name: mix mix_recipe_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.mix
    ADD CONSTRAINT mix_recipe_id_fkey FOREIGN KEY (recipe_id) REFERENCES nutrition.recipe(id) ON DELETE CASCADE;


--
-- Name: nrc_recipe_animal_input nrc_recipe_animal_input_nrc_recipe_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_animal_input
    ADD CONSTRAINT nrc_recipe_animal_input_nrc_recipe_fkey FOREIGN KEY (nrc_recipe) REFERENCES nutrition.nrc_recipe_param(nrc_recipe) ON DELETE CASCADE;


--
-- Name: nrc_recipe_item_feed_log nrc_recipe_feed_log_nrc_recipe_item_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_item_feed_log
    ADD CONSTRAINT nrc_recipe_feed_log_nrc_recipe_item_id_fkey FOREIGN KEY (nrc_recipe_item_id) REFERENCES nutrition.nrc_recipe_item(id);


--
-- Name: nrc_recipe_item nrc_recipe_item_nrc_recipe_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_item
    ADD CONSTRAINT nrc_recipe_item_nrc_recipe_fkey FOREIGN KEY (nrc_recipe) REFERENCES nutrition.nrc_recipe_param(nrc_recipe) ON DELETE CASCADE;


--
-- Name: nrc_recipe_location nrc_recipe_location_nrc_recipe_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_location
    ADD CONSTRAINT nrc_recipe_location_nrc_recipe_fkey FOREIGN KEY (nrc_recipe) REFERENCES nutrition.nrc_recipe_param(nrc_recipe) ON DELETE CASCADE;


--
-- Name: nrc_recipe_param nrc_recipe_param_create_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_param
    ADD CONSTRAINT nrc_recipe_param_create_userid_fkey FOREIGN KEY (create_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: nrc_recipe_param nrc_recipe_param_update_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.nrc_recipe_param
    ADD CONSTRAINT nrc_recipe_param_update_userid_fkey FOREIGN KEY (update_userid) REFERENCES wcauthentication.users(userid);


--
-- Name: recipe_item recipe_item_recipe_id_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.recipe_item
    ADD CONSTRAINT recipe_item_recipe_id_fkey FOREIGN KEY (recipe_id) REFERENCES nutrition.recipe(id) ON DELETE CASCADE;


--
-- Name: recipe_item recipe_item_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.recipe_item
    ADD CONSTRAINT recipe_item_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: recipe recipe_userid_fkey; Type: FK CONSTRAINT; Schema: nutrition; Owner: -
--

ALTER TABLE ONLY nutrition.recipe
    ADD CONSTRAINT recipe_userid_fkey FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid);


--
-- Name: machine_comment machine_comment_machine_id_fkey; Type: FK CONSTRAINT; Schema: picture; Owner: -
--

ALTER TABLE ONLY picture.machine_comment
    ADD CONSTRAINT machine_comment_machine_id_fkey FOREIGN KEY (machine_id) REFERENCES machinery.machine(id);


--
-- Name: machine machine_machine_id_fkey; Type: FK CONSTRAINT; Schema: picture; Owner: -
--

ALTER TABLE ONLY picture.machine
    ADD CONSTRAINT machine_machine_id_fkey FOREIGN KEY (machine_id) REFERENCES machinery.machine(id);


--
-- Name: classificationreport_temp classificationreport_temp_bovine_id_fkey; Type: FK CONSTRAINT; Schema: system; Owner: -
--

ALTER TABLE ONLY system.classificationreport_temp
    ADD CONSTRAINT classificationreport_temp_bovine_id_fkey FOREIGN KEY (bovine_id) REFERENCES bovinemanagement.bovine(id);


--
-- Name: users_in_groups user_fk; Type: FK CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.users_in_groups
    ADD CONSTRAINT user_fk FOREIGN KEY (userid) REFERENCES wcauthentication.users(userid) ON UPDATE RESTRICT ON DELETE CASCADE;


--
-- Name: users_in_groups users_in_groups_groupid_fkey; Type: FK CONSTRAINT; Schema: wcauthentication; Owner: -
--

ALTER TABLE ONLY wcauthentication.users_in_groups
    ADD CONSTRAINT users_in_groups_groupid_fkey FOREIGN KEY (groupid) REFERENCES wcauthentication.groups(name);


--
-- PostgreSQL database dump complete
--

