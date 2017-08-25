#include "include/qlSim.h"
namespace
{
class alpha62: public AlphaBase
{
    public:
    explicit alpha62(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")), 
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        Num1(cfg->getAttributeIntDefault("para1",20)),
	Num2(cfg->getAttributeIntDefault("para2",22)),
	Num3(cfg->getAttributeIntDefault("para3",9))
	
    {
        adv20.resize (GLOBAL::Dates.size());

        for (int di = 0; di< GLOBAL::Dates.size(); ++di)
        {
            adv20[di].resize(GLOBAL::Instruments.size(),nan);
        }
    }
    void generate(int di) override
    {
    	vector<float> rank1(GLOBAL::Instruments.size(),nan);
    	vector<float> rank2(GLOBAL::Instruments.size(),nan);
    	vector<float> open2_rank(GLOBAL::Instruments.size(),nan);
    	vector<float> med_rank(GLOBAL::Instruments.size(),nan);
    	vector<float> high_rank(GLOBAL::Instruments.size(),nan);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);
                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv20[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

		for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {	
            	vector<float> c3(Num3,0);
            	vector<float> c4(Num3,nan);


				for (int j =0; j<Num3; ++j)
	    		  {
	    		  	c3[j] = 0.0;
					for (int k = 0; k< Num2; ++k)
					{
		  				c3[j] += adv20[di-delay-k-j][ii];
					}

					c4[j] = vwap[di-delay-j][ii];
	      		  }

	      		rank1[ii] = QL_Oputils::corr(c3,c4);
	 	 	}
		}

		QL_Oputils::rank(rank1);
		
		 for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	open2_rank[ii] = 2.0 * open[di-delay][ii];
            	med_rank[ii] = 0.5 * (high[di-delay][ii]+low[di-delay][ii]);
            	high_rank[ii] = high[di-delay][ii];
            }
    	}

		QL_Oputils::rank(open2_rank);		
		QL_Oputils::rank(med_rank);
		QL_Oputils::rank(high_rank);

		 for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	if (open2_rank[ii]<(med_rank[ii]+high_rank[ii]))
            		{rank2[ii]=1;}
            	else
            		{rank2[ii]=0;}
            }
    	}

    	QL_Oputils::rank(rank2);


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
    
	    
	    		if (rank1[ii]<rank2[ii])
	    			{alpha[ii]=-1;}
	    		else
	    			{alpha[ii]= 0;}
	    }
        }
        return;
    }
    
    
    void checkPointSave(boost::archive::binary_oarchive &ar) 
    {
        ar & *this;
    }
    void checkPointLoad(boost::archive::binary_iarchive &ar)
    {
        ar & *this;
    }
    std::string version() const
    {
        return GLOBALCONST::VERSION;
    }
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int/*file_version*/)
        {
        }
	const QL_MATRIX<QL_FLOAT> &vwap;
	const QL_MATRIX<QL_FLOAT> &open;
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &low;
    vector<vector<float> > adv20;
        int Num1; 
	int Num2;
	int Num3;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha62(cfg);
        return str;
    }
}

