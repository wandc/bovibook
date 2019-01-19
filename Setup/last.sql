/* SQL */

/*owner*/
ALTER SCHEMA alpro OWNER TO r2d2;
ALTER SCHEMA bas OWNER TO r2d2;
ALTER SCHEMA batch OWNER TO r2d2;
ALTER SCHEMA bovinemanagement OWNER TO r2d2;
ALTER SCHEMA cropping OWNER TO r2d2;
ALTER SCHEMA documents OWNER TO r2d2;
ALTER SCHEMA gis OWNER TO r2d2;
ALTER SCHEMA hr OWNER TO r2d2;
ALTER SCHEMA intwebsite OWNER TO r2d2;
ALTER SCHEMA machinery OWNER TO r2d2;
ALTER SCHEMA misc OWNER TO r2d2;
ALTER SCHEMA nutrition OWNER TO r2d2;
ALTER SCHEMA pgagent OWNER TO r2d2;
ALTER SCHEMA picture OWNER TO r2d2;
ALTER SCHEMA system OWNER TO r2d2;
ALTER SCHEMA topology OWNER TO r2d2;
ALTER SCHEMA urban_feeder_foreign_tiere OWNER TO r2d2;
ALTER SCHEMA wcauthentication OWNER TO r2d2;

/*schema*/
GRANT ALL PRIVILEGES ON SCHEMA alpro TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA bas TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA batch TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA bovinemanagement TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA cropping TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA documents TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA gis TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA hr TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA intwebsite TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA machinery TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA misc TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA nutrition TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA pgagent TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA picture TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA system TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA topology TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA urban_feeder_foreign_tiere TO r2d2;
GRANT ALL PRIVILEGES ON SCHEMA wcauthentication TO r2d2;


/*tables*/
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA alpro TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA bas TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA batch TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA bovinemanagement TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA cropping TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA documents TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA gis TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA hr TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA intwebsite TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA machinery TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA misc TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA nutrition TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA pgagent TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA picture TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA system TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA topology TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA urban_feeder_foreign_tiere TO r2d2;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA wcauthentication TO r2d2;


/*sequences*/
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA alpro TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA bas TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA batch TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA bovinemanagement TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA cropping TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA documents TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA gis TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA hr TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA intwebsite TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA machinery TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA misc TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA nutrition TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA pgagent TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA picture TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA system TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA topology TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA urban_feeder_foreign_tiere TO r2d2;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA wcauthentication TO r2d2;

/*functions*/
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA alpro TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA bas TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA batch TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA bovinemanagement TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA cropping TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA documents TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA gis TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA hr TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA intwebsite TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA machinery TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA misc TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA nutrition TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA pgagent TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA picture TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA system TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA topology TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA urban_feeder_foreign_tiere TO r2d2;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA wcauthentication TO r2d2;



--
-- Data for Name: groups; Type: TABLE DATA; Schema: wcauthentication; Owner: david
--

INSERT INTO wcauthentication.groups VALUES (1, 'employee                        ');
INSERT INTO wcauthentication.groups VALUES (2, 'herdsperson                     ');
INSERT INTO wcauthentication.groups VALUES (3, 'cropsperson                     ');
INSERT INTO wcauthentication.groups VALUES (4, 'manager                         ');
INSERT INTO wcauthentication.groups VALUES (5, 'owner                           ');
INSERT INTO wcauthentication.groups VALUES (6, 'admin                           ');
INSERT INTO wcauthentication.groups VALUES (7, 'veterinary                      ');
INSERT INTO wcauthentication.groups VALUES (8, 'embryo_implant                  ');
INSERT INTO wcauthentication.groups VALUES (9, 'nutritionist                    ');
INSERT INTO wcauthentication.groups VALUES (10, 'manure                          ');
INSERT INTO wcauthentication.groups VALUES (11, 'spray                           ');
INSERT INTO wcauthentication.groups VALUES (12, 'debug                           ');
INSERT INTO wcauthentication.groups VALUES (13, 'agronomist                      ');
INSERT INTO wcauthentication.groups VALUES (14, 'secure_basic                    ');


/* add admin user */
INSERT INTO wcauthentication.oauth_clients (client_id,client_secret,redirect_uri,scope,user_id) VALUES ('admin','f5f0d2bc73d70b8356b54e584661a44d','null','null','null');
INSERT INTO wcauthentication.users (firstname,lastname,userid,email,active,onfarm)VALUES ('admin','user','admin','admin@example.com','true','true');
INSERT INTO wcauthentication.users_in_groups (userid,groupid) VALUES ('admin','admin');



--
-- Data for Name: ajax; Type: TABLE DATA; Schema: intwebsite; Owner: david
--

INSERT INTO intwebsite.ajax VALUES (10000, 'sideBarInfo.inc', 'functions');
INSERT INTO intwebsite.ajax VALUES (10001, 'footer.inc', 'template');
INSERT INTO intwebsite.ajax VALUES (10002, 'verticalMenu.inc', 'functions');
INSERT INTO intwebsite.ajax VALUES (10003, 'search.inc', 'sitePages/structure');
INSERT INTO intwebsite.ajax VALUES (10004, 'googleMailSite.inc', 'functions');
INSERT INTO intwebsite.ajax VALUES (10005, 'header.inc', 'template');
INSERT INTO intwebsite.ajax VALUES (10006, 'header.inc', 'template');


--
-- Data for Name: ajax_security; Type: TABLE DATA; Schema: intwebsite; Owner: david
--

INSERT INTO intwebsite.ajax_security VALUES (1, 10000, 'owner                                                           ');
INSERT INTO intwebsite.ajax_security VALUES (2, 10001, 'employee                                                        ');
INSERT INTO intwebsite.ajax_security VALUES (3, 10002, 'employee                                                        ');
INSERT INTO intwebsite.ajax_security VALUES (4, 10003, 'employee                                                        ');
INSERT INTO intwebsite.ajax_security VALUES (5, 10001, 'nutritionist                                                    ');
INSERT INTO intwebsite.ajax_security VALUES (6, 10002, 'nutritionist                                                    ');
INSERT INTO intwebsite.ajax_security VALUES (7, 10004, 'employee                                                        ');
INSERT INTO intwebsite.ajax_security VALUES (8, 10005, 'employee                                                        ');
INSERT INTO intwebsite.ajax_security VALUES (9, 10006, 'owner                                                           ');
INSERT INTO intwebsite.ajax_security VALUES (10, 10006, 'nutritionist                                                    ');
INSERT INTO intwebsite.ajax_security VALUES (11, 10006, 'employee                                                        ');


--
-- Data for Name: page; Type: TABLE DATA; Schema: intwebsite; Owner: david
--

INSERT INTO intwebsite.page VALUES (1, 'home.inc', 'Home Page', 'structure', 0, 1, true, true);
INSERT INTO intwebsite.page VALUES (38, 'movementLocationList.inc', 'Location List', 'bovineManagement', 15, 2, true, true);
INSERT INTO intwebsite.page VALUES (18, '', 'Transition', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (63, 'feet.inc', 'Feet', 'medical', 24, 4, true, true);
INSERT INTO intwebsite.page VALUES (65, 'salePrice.inc', 'Sale Pricing', 'bovineManagement', 46, 10, true, true);
INSERT INTO intwebsite.page VALUES (69, 'bagConsumption.inc', 'Bag Consumption', 'nutrition', 67, 3, true, true);
INSERT INTO intwebsite.page VALUES (68, 'moistureForageTest.inc', 'Bag Moisture', 'nutrition', 67, 2, true, true);
INSERT INTO intwebsite.page VALUES (64, 'medicalCase.inc', 'Treatment', 'medical', 24, 2, true, true);
INSERT INTO intwebsite.page VALUES (70, 'timeSheet.inc', 'Time Sheet', 'hr', 47, 1, true, true);
INSERT INTO intwebsite.page VALUES (74, 'workSchedule.inc', 'Work Schedule', 'hr', 47, 2, true, true);
INSERT INTO intwebsite.page VALUES (79, 'medicineScheduled.inc', 'Medicine Scheduled', 'medical', 24, 8, true, true);
INSERT INTO intwebsite.page VALUES (81, 'video.inc', 'Video', 'misc', 46, 11, true, true);
INSERT INTO intwebsite.page VALUES (89, 'sop.inc', 'Standard Operating Procedures', 'misc', 46, 155, true, true);
INSERT INTO intwebsite.page VALUES (96, 'feedUsageProjection.inc', 'Feed Usage Projection', 'nutrition', 67, 30, true, true);
INSERT INTO intwebsite.page VALUES (90, 'preCalving.inc', 'Pre-Calving Tasks', 'transition', 18, 3, true, true);
INSERT INTO intwebsite.page VALUES (103, 'pictureNeeded.inc', 'Need Picture', 'reports', 50, 999, true, true);
INSERT INTO intwebsite.page VALUES (84, 'vaccination.inc', 'Vaccination', 'medical', 24, 12, true, true);
INSERT INTO intwebsite.page VALUES (97, 'classificationDay.inc', 'Classifier List', 'reports', 50, 88, true, true);
INSERT INTO intwebsite.page VALUES (60, 'cullList.inc', 'Cull List', 'bovineManagement', 46, 9, true, true);
INSERT INTO intwebsite.page VALUES (98, 'herdCull.inc', 'Herd Cull', 'bovineManagement', 15, 77, true, true);
INSERT INTO intwebsite.page VALUES (85, 'reproReport.inc', 'Repro Report', 'reports', 50, 5, true, true);
INSERT INTO intwebsite.page VALUES (99, 'medicineAdmin.inc', 'Medicine Admin', 'medical', 24, 55, true, true);
INSERT INTO intwebsite.page VALUES (88, 'task.inc', 'Tasks', 'misc', 46, 50, true, true);
INSERT INTO intwebsite.page VALUES (73, 'milkingStats.inc', 'Milking Stats', 'reports', 50, 0, true, true);
INSERT INTO intwebsite.page VALUES (86, 'feedAdmin.inc', 'Feed Admin', 'nutrition', 67, 12, true, true);
INSERT INTO intwebsite.page VALUES (92, 'tmrHistorical.inc', 'TMR Historical', 'nutrition', 67, 30, true, true);
INSERT INTO intwebsite.page VALUES (104, 'cqm.inc', 'CQM', 'reports', 50, 1, true, true);
INSERT INTO intwebsite.page VALUES (105, 'movementSortGate.inc', 'Sort Gate', 'bovineManagement', 15, 400, true, true);
INSERT INTO intwebsite.page VALUES (106, 'bovinePicture.inc', 'Pictures', 'reports', 50, 400, true, true);
INSERT INTO intwebsite.page VALUES (78, 'documents.inc', 'Documents', 'machinery', 108, 5, true, true);
INSERT INTO intwebsite.page VALUES (87, 'purchases.inc', 'Purchases', 'machinery', 108, 15, true, true);
INSERT INTO intwebsite.page VALUES (111, 'deworming.inc', 'Deworming', 'medical', 24, 500, true, true);
INSERT INTO intwebsite.page VALUES (112, 'commodityPrices.inc', 'Commodity Prices', 'nutrition', 67, 500, true, true);
INSERT INTO intwebsite.page VALUES (51, 'reportsRFIDStats.inc', 'RFID Reader Stats', 'reports', 50, 1, true, true);
INSERT INTO intwebsite.page VALUES (56, 'reportsNumberMilkingByDate.inc', 'Number Milking', 'reports', 50, 3, true, true);
INSERT INTO intwebsite.page VALUES (113, 'animalInventory.inc', 'Animal Inventory', 'reports', 50, 999, true, true);
INSERT INTO intwebsite.page VALUES (55, 'testPage.inc', 'Test Page', 'misc', 46, 20, true, true);
INSERT INTO intwebsite.page VALUES (44, 'transitionDryoff.inc', 'Dry-Off', 'transition', 18, 2, true, true);
INSERT INTO intwebsite.page VALUES (43, 'transitionCalving.inc', 'Calving', 'transition', 18, 1, true, true);
INSERT INTO intwebsite.page VALUES (114, 'nrc2001.inc', 'NRC 2001', 'nutrition', 67, 500, true, true);
INSERT INTO intwebsite.page VALUES (4, 'field.inc', 'Fields', 'cropping', 45, 2, false, true);
INSERT INTO intwebsite.page VALUES (58, 'cornHeatUnits.inc', 'Corn Heat Units', 'cropping', 45, 4, true, true);
INSERT INTO intwebsite.page VALUES (101, 'manure.inc', 'Manure Spreading', 'cropping', 45, 8, true, true);
INSERT INTO intwebsite.page VALUES (59, 'soilSample.inc', 'Soil Samples', 'cropping', 45, 5, true, true);
INSERT INTO intwebsite.page VALUES (117, 'machineryAdmin.inc', 'Machinery Admin', 'machinery', 108, 200, true, true);
INSERT INTO intwebsite.page VALUES (61, 'quota.inc', 'Quota Holdings', 'misc', 46, 10, true, true);
INSERT INTO intwebsite.page VALUES (26, 'calfName.inc', 'Calf Naming', 'heifer', 25, 2, true, true);
INSERT INTO intwebsite.page VALUES (72, 'sickGroup.inc', 'Sick &amp; Fresh Hot List', 'medical', 24, 1, true, true);
INSERT INTO intwebsite.page VALUES (50, '', 'Reports', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (67, '', 'Nutrition', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (47, '', 'HR', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (46, '', 'Misc', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (108, '', 'Machinery', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (45, '', 'Cropping', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (118, 'genetics.inc', 'Genetics', 'bovineManagement', 7, 9999, true, true);
INSERT INTO intwebsite.page VALUES (25, '', 'Heifers', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (24, '', 'Medical', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (119, 'vetToCheck.inc', 'Vet To Check', 'medical', 24, 101, true, true);
INSERT INTO intwebsite.page VALUES (120, 'calfRegistration.inc', 'Calf Registration', 'heifer', 25, 3, true, true);
INSERT INTO intwebsite.page VALUES (121, 'familyTree.inc', 'Family Tree', 'reports', 50, 900, true, true);
INSERT INTO intwebsite.page VALUES (122, 'search.inc', 'Search Results', 'structure', NULL, 1, true, true);
INSERT INTO intwebsite.page VALUES (144, 'fertilizer.inc', 'Fertilizer', 'cropping', 45, 43, true, true);
INSERT INTO intwebsite.page VALUES (115, 'bodyConditionScore.inc', 'Body Condition Score', 'medical', 24, 200, true, true);
INSERT INTO intwebsite.page VALUES (22, 'bulkTankPickup.inc', 'Milk Pickup', 'misc', 46, 111, true, true);
INSERT INTO intwebsite.page VALUES (123, 'employeePerformance.inc', 'Performance Reports', 'hr', 47, 200, true, true);
INSERT INTO intwebsite.page VALUES (107, 'fieldQuery.inc', 'Field Query', 'cropping', NULL, NULL, false, true);
INSERT INTO intwebsite.page VALUES (95, 'machineQuery.inc', 'Machine Query', 'machinery', NULL, NULL, true, true);
INSERT INTO intwebsite.page VALUES (14, 'map.inc', 'Map', 'cropping', 45, 44, true, true);
INSERT INTO intwebsite.page VALUES (124, 'rowCropping.inc', 'Row Cropping', 'cropping', 45, 999, true, true);
INSERT INTO intwebsite.page VALUES (53, 'calfFeeding.inc', 'Calf Feeding', 'heifer', 25, 1, true, true);
INSERT INTO intwebsite.page VALUES (30, 'estrusBreedings.inc', 'Breedings', 'reproduction', 7, 3, true, true);
INSERT INTO intwebsite.page VALUES (11, 'weather.inc', 'Weather', 'cropping', 45, 99999, true, true);
INSERT INTO intwebsite.page VALUES (34, 'estrusEmbryo.inc', 'Embryo Inventory', 'reproduction', 7, 7, true, true);
INSERT INTO intwebsite.page VALUES (77, 'estrusHormone.inc', 'Repro Hormone', 'reproduction', 7, 11, true, true);
INSERT INTO intwebsite.page VALUES (31, 'estrusKamar.inc', 'Kamar', 'reproduction', 7, 4, true, true);
INSERT INTO intwebsite.page VALUES (35, 'estrusPregnancyCheck.inc', 'Pregnancy Check', 'reproduction', 7, 8, true, true);
INSERT INTO intwebsite.page VALUES (32, 'estrusProtocol.inc', 'Protocol', 'reproduction', 7, 5, true, true);
INSERT INTO intwebsite.page VALUES (33, 'estrusSemen.inc', 'Semen Inventory', 'reproduction', 7, 6, true, true);
INSERT INTO intwebsite.page VALUES (28, 'estrusHeats.inc', 'Heats', 'reproduction', 7, 1, true, true);
INSERT INTO intwebsite.page VALUES (7, '', 'Reproduction', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (126, 'dnaTest.inc', 'DNA Testing', 'heifer', 25, 990, true, true);
INSERT INTO intwebsite.page VALUES (127, 'herdProjections.inc', 'Herd Projections', 'reports', 50, 22, true, true);
INSERT INTO intwebsite.page VALUES (128, 'parlorMilking.inc', 'Parlor Milking', 'reports', 50, 999, true, true);
INSERT INTO intwebsite.page VALUES (21, 'valactaLatestTest.inc', 'Valacta Test Results', 'reports', 50, 100, true, true);
INSERT INTO intwebsite.page VALUES (3, 'userAdmin.inc', 'User Administration', 'hr', 47, 99, true, true);
INSERT INTO intwebsite.page VALUES (54, 'pageSecurityAdmin.inc', 'Page Security', 'hr', 47, 2, true, true);
INSERT INTO intwebsite.page VALUES (116, 'groupChange.inc', 'Group Change Report', 'bovineManagement', 15, 100, true, true);
INSERT INTO intwebsite.page VALUES (15, '', 'Movement', '', 1, NULL, true, true);
INSERT INTO intwebsite.page VALUES (129, 'profitProfiler.inc', 'Profit Profiler', 'reports', 50, 9999, true, true);
INSERT INTO intwebsite.page VALUES (145, 'videoCalfBarn.inc', 'Video Calf Barn', 'misc', 46, 13, true, true);
INSERT INTO intwebsite.page VALUES (37, 'movementIndividual.inc', 'Animal Move', 'bovineManagement', 15, 1, true, true);
INSERT INTO intwebsite.page VALUES (130, 'video2.inc', 'Video2', 'misc', 46, 12, true, true);
INSERT INTO intwebsite.page VALUES (131, 'historicalHours.inc', 'Historical Hours Report', 'hr', 47, 9999, true, true);
INSERT INTO intwebsite.page VALUES (133, 'bagSample.inc', 'Bag Forage/Feed Sample', 'nutrition', 67, 9000, true, true);
INSERT INTO intwebsite.page VALUES (66, 'agbagadmin.inc', 'Silage Bag', 'nutrition', 67, 1, true, true);
INSERT INTO intwebsite.page VALUES (6, 'bovineQuery.php', 'Animal Query', 'bovineManagement', NULL, NULL, true, true);
INSERT INTO intwebsite.page VALUES (134, 'longProcesses.inc', 'Long Processes', 'misc', 46, 2500, true, true);
INSERT INTO intwebsite.page VALUES (135, 'dBaseReports.inc', 'dBase Reports', 'reports', 50, 999, true, true);
INSERT INTO intwebsite.page VALUES (136, 'sireChoice.inc', 'Sire Choice', 'bovineManagement', 7, 888, true, true);
INSERT INTO intwebsite.page VALUES (137, 'beefReport.inc', 'Beef Market Report', 'reports', 50, 999, true, true);
INSERT INTO intwebsite.page VALUES (29, 'estrusTwentyOneDayCalendar.inc', '21 Day Calendar', 'reproduction', 7, 5, true, true);
INSERT INTO intwebsite.page VALUES (93, 'tmrInTractor.inc', 'TMR In Tractor', 'nutrition', 67, 55, true, true);
INSERT INTO intwebsite.page VALUES (140, 'estrusDetector.inc', 'Estrus Detect', 'reproduction', 7, 100000, true, true);
INSERT INTO intwebsite.page VALUES (139, 'tmrRecipe.inc', 'TMR Recipe', 'nutrition', 67, 100, true, true);
INSERT INTO intwebsite.page VALUES (143, 'debug.inc', 'Debug', 'misc', 46, 10000, true, true);
INSERT INTO intwebsite.page VALUES (52, 'valactaTestInput.inc', 'Valacta Tester Input Report', 'reports', 50, 99, true, true);
INSERT INTO intwebsite.page VALUES (146, 'tmrRecipeNew.inc', 'TMR Recipe NEW', 'nutrition', 67, 1, true, true);
INSERT INTO intwebsite.page VALUES (23, 'semenChooser.inc', 'Semen Chooser', 'reproduction', 7, 50, true, true);
INSERT INTO intwebsite.page VALUES (132, 'videoArchive.inc', 'Video Archive', 'misc', 46, 14, false, true);
INSERT INTO intwebsite.page VALUES (142, 'building.inc', 'Building Automation', 'misc', 46, 25, true, true);
INSERT INTO intwebsite.page VALUES (147, 'tmrReport.inc', 'TMR Report', 'reports', 50, 5, true, true);
INSERT INTO intwebsite.page VALUES (148, 'shurgainReport.inc', 'Shur-Gain Report', 'nutrition', 67, 1000, true, true);
INSERT INTO intwebsite.page VALUES (13, 'earTag.inc', 'Animal Ear Tag', 'medical', 24, 15, true, true);


--
-- Data for Name: page_security; Type: TABLE DATA; Schema: intwebsite; Owner: david
--

INSERT INTO intwebsite.page_security VALUES (27, 1, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (1, 1, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (2, 1, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (163, 1, 'nutritionist                                                    ');
INSERT INTO intwebsite.page_security VALUES (247, 1, 'agronomist                                                      ');
INSERT INTO intwebsite.page_security VALUES (3, 1, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (8, 3, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (210, 4, 'cropsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (9, 4, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (295, 142, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (7, 6, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (6, 6, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (38, 6, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (250, 7, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (17, 11, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (82, 11, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (83, 11, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (223, 13, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (224, 13, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (19, 13, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (20, 14, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (246, 14, 'agronomist                                                      ');
INSERT INTO intwebsite.page_security VALUES (84, 14, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (24, 14, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (26, 15, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (265, 15, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (22, 15, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (42, 18, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (36, 18, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (30, 18, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (88, 21, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (86, 21, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (35, 21, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (89, 22, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (41, 22, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (40, 22, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (43, 23, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (254, 24, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (267, 24, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (44, 24, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (48, 25, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (45, 25, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (46, 25, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (47, 25, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (49, 26, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (51, 28, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (73, 28, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (90, 28, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (91, 29, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (92, 29, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (52, 29, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (74, 30, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (53, 30, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (93, 30, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (75, 31, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (54, 31, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (94, 31, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (76, 32, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (56, 33, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (57, 34, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (151, 35, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (150, 35, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (96, 37, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (98, 37, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (60, 37, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (97, 38, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (99, 38, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (61, 38, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (102, 43, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (62, 43, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (64, 44, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (63, 44, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (65, 45, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (264, 45, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (66, 46, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (255, 46, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (266, 47, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (67, 47, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (253, 50, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (71, 51, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (72, 52, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (138, 52, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (78, 53, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (79, 54, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (103, 55, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (104, 56, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (106, 58, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (107, 59, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (108, 60, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (109, 61, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (140, 63, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (111, 63, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (149, 64, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (112, 64, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (148, 64, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (116, 65, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (117, 66, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (251, 67, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (119, 68, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (118, 68, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (120, 68, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (164, 69, 'nutritionist                                                    ');
INSERT INTO intwebsite.page_security VALUES (121, 69, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (123, 69, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (122, 69, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (128, 70, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (124, 70, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (126, 72, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (268, 72, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (127, 73, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (271, 73, 'nutritionist                                                    ');
INSERT INTO intwebsite.page_security VALUES (130, 74, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (129, 74, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (134, 77, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (137, 77, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (136, 78, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (135, 78, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (142, 79, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (144, 81, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (145, 81, 'manager                                                         ');
INSERT INTO intwebsite.page_security VALUES (305, 96, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (199, 84, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (152, 84, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (153, 84, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (154, 85, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (155, 86, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (156, 87, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (157, 88, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (168, 88, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (167, 88, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (169, 89, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (170, 89, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (158, 89, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (159, 90, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (165, 92, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (166, 92, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (172, 93, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (175, 95, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (202, 95, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (212, 95, 'cropsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (176, 95, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (180, 96, 'nutritionist                                                    ');
INSERT INTO intwebsite.page_security VALUES (177, 96, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (178, 96, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (181, 97, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (182, 97, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (186, 98, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (183, 98, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (184, 98, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (185, 98, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (189, 99, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (188, 99, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (193, 101, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (192, 101, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (197, 103, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (198, 103, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (245, 103, 'manager                                                         ');
INSERT INTO intwebsite.page_security VALUES (196, 103, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (200, 104, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (209, 104, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (201, 104, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (203, 105, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (204, 105, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (270, 105, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (205, 106, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (206, 106, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (208, 107, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (207, 107, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (252, 108, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (218, 111, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (217, 111, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (220, 112, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (269, 112, 'nutritionist                                                    ');
INSERT INTO intwebsite.page_security VALUES (219, 112, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (221, 113, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (222, 113, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (226, 114, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (225, 114, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (227, 115, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (233, 116, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (228, 116, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (229, 117, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (230, 117, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (231, 118, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (232, 118, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (234, 119, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (235, 119, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (237, 120, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (236, 120, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (238, 121, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (239, 121, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (241, 122, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (240, 122, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (242, 122, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (244, 123, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (243, 123, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (248, 124, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (249, 124, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (257, 126, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (256, 126, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (258, 127, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (259, 127, 'manager                                                         ');
INSERT INTO intwebsite.page_security VALUES (260, 128, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (261, 129, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (262, 130, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (263, 131, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (272, 132, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (273, 132, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (274, 133, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (275, 133, 'manager                                                         ');
INSERT INTO intwebsite.page_security VALUES (276, 134, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (277, 134, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (278, 135, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (279, 135, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (280, 136, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (281, 136, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (282, 137, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (283, 137, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (284, 137, 'nutritionist                                                    ');
INSERT INTO intwebsite.page_security VALUES (285, 139, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (286, 139, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (287, 139, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (288, 139, 'nutritionist                                                    ');
INSERT INTO intwebsite.page_security VALUES (80, 35, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (289, 140, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (290, 140, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (291, 101, 'manure                                                          ');
INSERT INTO intwebsite.page_security VALUES (296, 142, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (297, 142, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (298, 142, 'secure_basic                                                    ');
INSERT INTO intwebsite.page_security VALUES (299, 1, 'secure_basic                                                    ');
INSERT INTO intwebsite.page_security VALUES (300, 143, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (301, 144, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (302, 144, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (303, 144, 'manure                                                          ');
INSERT INTO intwebsite.page_security VALUES (304, 140, 'employee                                                        ');
INSERT INTO intwebsite.page_security VALUES (306, 145, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (307, 145, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (308, 53, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (309, 146, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (310, 146, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (311, 146, 'herdsperson                                                     ');
INSERT INTO intwebsite.page_security VALUES (312, 147, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (313, 147, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (314, 67, 'nutritionist                                                    ');
INSERT INTO intwebsite.page_security VALUES (315, 148, 'admin                                                           ');
INSERT INTO intwebsite.page_security VALUES (316, 148, 'owner                                                           ');
INSERT INTO intwebsite.page_security VALUES (317, 96, 'employee                                                        ');


--
-- Data for Name: sse; Type: TABLE DATA; Schema: intwebsite; Owner: david
--

INSERT INTO intwebsite.sse VALUES (20007, 'AlproSortAndMovementSync.inc', 'phpCronScripts', 'Alpro Sort Gate Sync');
INSERT INTO intwebsite.sse VALUES (20002, 'AggregateSiteData.inc', 'phpCronScripts', 'Aggregate Site Data Download (CDN/HOL)');
INSERT INTO intwebsite.sse VALUES (20006, 'MQMParser.php', 'phpCronScripts', 'NB Bulk Tank Sample From MQM');
INSERT INTO intwebsite.sse VALUES (20000, 'CDNProgenyInbreedingMulti.php', 'phpCronScripts', 'CDN Progeny Inbreeding');


--
-- Data for Name: sse_security; Type: TABLE DATA; Schema: intwebsite; Owner: david
--

INSERT INTO intwebsite.sse_security VALUES (2000, 20000, 'owner                                                           ');
INSERT INTO intwebsite.sse_security VALUES (2001, 20000, 'admin                                                           ');
INSERT INTO intwebsite.sse_security VALUES (2004, 20002, 'owner                                                           ');
INSERT INTO intwebsite.sse_security VALUES (2011, 20002, 'admin                                                           ');
INSERT INTO intwebsite.sse_security VALUES (2012, 20006, 'admin                                                           ');
INSERT INTO intwebsite.sse_security VALUES (2013, 20006, 'owner                                                           ');
INSERT INTO intwebsite.sse_security VALUES (2014, 20007, 'admin                                                           ');
INSERT INTO intwebsite.sse_security VALUES (2015, 20007, 'owner                                                           ');


--
-- Name: ajax_id_seq; Type: SEQUENCE SET; Schema: intwebsite; Owner: david
--

SELECT pg_catalog.setval('intwebsite.ajax_id_seq', 10006, true);


--
-- Name: ajax_security_id_seq; Type: SEQUENCE SET; Schema: intwebsite; Owner: david
--

SELECT pg_catalog.setval('intwebsite.ajax_security_id_seq', 11, true);


--
-- Name: page_pageid_seq; Type: SEQUENCE SET; Schema: intwebsite; Owner: david
--

SELECT pg_catalog.setval('intwebsite.page_pageid_seq', 148, true);


--
-- Name: page_security_id_seq; Type: SEQUENCE SET; Schema: intwebsite; Owner: david
--

SELECT pg_catalog.setval('intwebsite.page_security_id_seq', 318, true);


--
-- Name: sse_id_seq; Type: SEQUENCE SET; Schema: intwebsite; Owner: david
--

SELECT pg_catalog.setval('intwebsite.sse_id_seq', 20007, true);


--
-- Name: sse_security_id_seq; Type: SEQUENCE SET; Schema: intwebsite; Owner: david
--

SELECT pg_catalog.setval('intwebsite.sse_security_id_seq', 2015, true);


--
-- PostgreSQL database dump complete
--
