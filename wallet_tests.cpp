// Copyright (c) 2012-2014 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
// 하이루
#include "wallet.h"

#include <set>
#include <stdint.h>
#include <utility>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>
#include "json/json_spirit_utils.h"
#include "json/json_spirit_value.h"
#include "rpcserver.h"

// how many times to run all the tests to have a chance to catch errors that only show up with particular random shuffles
#define RUN_TESTS 100

// some tests fail 1% of the time due to bad luck.
// we repeat those tests this many times and only complain if all iterations of the test fail
#define RANDOM_REPEATS 5

using namespace json_spirit;
using namespace std;

typedef set<pair<const CWalletTx*,unsigned int> > CoinSet;

BOOST_AUTO_TEST_SUITE(wallet_tests)

static CWallet wallet;
static vector<COutput> vCoins;
/*                                   
//////////////////////////////////// Original Version ////////////////////////////////////////////////
static void add_coin(const CAmount& nValue, int nAge = 6 * 24, bool fIsFromMe = false, int nInput = 0)
{
	static int nextLockTime = 0;
	CMutableTransaction tx;
	tx.nLockTime = nextLockTime++;        // so all transactions get different hashes
	tx.vout.resize(nInput + 1);
	tx.vout[nInput].nValue = nValue;
	if (fIsFromMe)
	{
		// IsFromMe() returns (GetDebit() > 0), and GetDebit() is 0 if vin.empty(),
		// so stop vin being empty, and cache a non-zero Debit to fake out IsFromMe()
		tx.vin.resize(1);
	}
	CWalletTx* wtx = new CWalletTx(&wallet, tx);
	if (fIsFromMe)
	{
		wtx->fDebitCached = true;
		wtx->nDebitCached = 1;
	}
	COutput output(wtx, nInput, nAge, true);
	vCoins.push_back(output);
	if (fIsFromMe)
	{
		// IsFromMe() returns (GetDebit() > 0), and GetDebit() is 0 if vin.empty(),
		// so stop vin being empty, and cache a non-zero Debit to fake out IsFromMe()
		tx.vin.resize(1);
	}
	CWalletTx* wtx = new CWalletTx(&wallet, tx);
	if (fIsFromMe)
	{
		wtx->fDebitCached = true;
		wtx->nDebitCached = 1;
	}
	COutput output(wtx, nInput, nAge, true);
	vCoins.push_back(output);
}
*/

static void add_prod(int64_t id, int64_t nCountryCode, int64_t nZipCode, int nAge = 6 * 24, bool fIsFromMe = false, int nInput = 0) {
	static int nextLockTime = 0;
	CMutableTransaction tx;
	tx.nLockTime = nextLockTime++;        // so all transactions get different hashes
	tx.vout.resize(nInput + 1);

	tx.vout[nInput].txInfo.nID = id; //tx.vout[nInput].nValue = nValue;
	tx.vout[nInput].txInfo.nCountryCode = nCountryCode;
	tx.vout[nInput].txInfo.nZipCode = nZipCode;

	if (fIsFromMe)
	{
		// IsFromMe() returns (GetDebit() > 0), and GetDebit() is 0 if vin.empty(),
		// so stop vin being empty, and cache a non-zero Debit to fake out IsFromMe()
		tx.vin.resize(1);
	}
	CWalletTx* wtx = new CWalletTx(&wallet, tx);
	/*
	if (fIsFromMe)
	{
		wtx->fDebitCached = true;
		wtx->nDebitCached = 1;
	}
	*/
	COutput output(wtx, nInput, nAge, true);
	vCoins.push_back(output);
}

static void empty_wallet(void)
{
    BOOST_FOREACH(COutput output, vCoins)
        delete output.tx;
    vCoins.clear();
}

static bool equal_sets(CoinSet a, CoinSet b)
{
    pair<CoinSet::iterator, CoinSet::iterator> ret = mismatch(a.begin(), a.end(), b.begin());
    return ret.first == a.end() && ret.second == b.end();
}

BOOST_AUTO_TEST_CASE(coin_selection_tests)
{
	CoinSet setCoinsRet, setCoinsRet2;
	CAmount nValueRet; // ->> CAmount는 결국 int64_t 다!!!!!!!!!!!!!!!!!!!!!!!!!!!

	LOCK(wallet.cs_wallet);

	// test multiple times to allow for differences in the shuffle order
	for (int i = 0; i < RUN_TESTS; i++)
	{
		empty_wallet();

		CTxInfo tmp_1(1, 1, 1);
		// {ID = 1, CountryCode = 1, ZipCode = 1}에 해당하는 제품을 추가하지 않았기 때문에 SelectCoinsMinConf()의 return값은 false.
		BOOST_CHECK(!wallet.SelectCoinsMinConf(tmp_1, vCoins, setCoinsRet)); 
		add_prod(1, 1, 1); // {ID = 1, CountryCode = 1, ZipCode = 1} 추가.
		// {ID = 1, CountryCode = 1, ZipCode = 1}에 해당하는 제품을 추가했기 때문에 SelectCoinsMinConf()의 return값은 true.
		BOOST_CHECK(wallet.SelectCoinsMinConf(tmp_1, vCoins, setCoinsRet));
		// add_prod(1, 1, 1); // {ID = 1, CountryCode = 1, ZipCode = 1}와 동일한 제품을 한번 더 추가했을 경우에 추가되지 않아야함.

		CTxInfo tmp_2(2, 2, 2);
		BOOST_CHECK(!wallet.SelectCoinsMinConf(tmp_2, vCoins, setCoinsRet));
		add_prod(2, 2, 2);
		BOOST_CHECK(wallet.SelectCoinsMinConf(tmp_2, vCoins, setCoinsRet));



	}
}

BOOST_AUTO_TEST_CASE(gen_new_product_tests)
{
	const Array& params1 = { "{“ID”:3,“countryCode”:3,“zipCode”:3}" };
	Value v1;
	v1 = gen_new_product(params1, false);

	const Array& params2 = { "1", "true" };
	Value v2;
	v2 = set_generate(params2, false);
	if (v2 == NULL &&){

	}
	
}

BOOST_AUTO_TEST_SUITE_END()