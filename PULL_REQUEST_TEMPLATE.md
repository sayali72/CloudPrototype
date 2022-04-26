Refer [PR Checklist](https://ushrauto-my.sharepoint.com/:w:/p/jclements/Ef22QBLV-k5CthT4nWSuxEkBOAJ5BlsOZOF7F-oSxTwpLg?e=paYIAx) for detailed explanation

|Done|N/A|Checklist items|
|-|-|-|
|Regression Test, UDBI Equality Test|
|-[]-[]|Regression test passed|
|[]|[]|Did not notice any significant setback in executing regression test even if it is passed|
|[]|[]|Any changes to regression test is taken into account|
|[]|[]|Assessed the impact of adding new test case on overall regression execution time|
|[]|[]|Any changes to UDBI class eq method is taken into account|
|Query|
|[]|[]|All indexes updated|
|[]|[]|Evaluated the effect on the readability and performance if the query uses JOIN/Subquery/CTE|
|[]|[]|Database connection closed for dblink query|
|[]|[]|Considered adding Foreign Data Wrapper|
|[]|[]|publish_prep deleted and recreated to test the changes|
|[]|[]|Added table to the appropriate list in publish_udbi.py if the change involves adding new table to publish_prep|
|[]|[]|Considered different ways of optimizing the query|
|[]|[]|Ran EXPLAIN on the query|
|[]|[]|Verified if query is executed on the appropriate database|
|[]|[]|Properly formatted query (indentation and capitalization of keywords)|
