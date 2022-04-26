Refer [PR Checklist](https://ushrauto-my.sharepoint.com/:w:/p/jclements/Ef22QBLV-k5CthT4nWSuxEkBOAJ5BlsOZOF7F-oSxTwpLg?e=paYIAx) for detailed explanation

|Done|N/A|Checklist items|
|-|-|-|
|Regression Test, UDBI Equality Test|
| <ul><li>- [x] </li></ul>| <ul><li>- [ ] </li></ul> |Regression test passed|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Did not notice any significant setback in executing regression test even if it is passed|
| <ul><li>- [x] </li></ul>| <ul><li>- [ ] </li></ul> |Any changes to regression test is taken into account|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Assessed the impact of adding new test case on overall regression execution time|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Any changes to UDBI class eq method is taken into account|
|Query|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |All indexes updated|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Evaluated the effect on the readability and performance if the query uses JOIN/Subquery/CTE|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Database connection closed for dblink query|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Considered adding Foreign Data Wrapper|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |publish_prep deleted and recreated to test the changes|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Added table to the appropriate list in publish_udbi.py if the change involves adding new table to publish_prep|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Considered different ways of optimizing the query|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Ran EXPLAIN on the query|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Verified if query is executed on the appropriate database|
| <ul><li>- [ ] </li></ul>| <ul><li>- [x] </li></ul> |Properly formatted query (indentation and capitalization of keywords)|
