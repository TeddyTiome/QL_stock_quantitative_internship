#include "include/qlSim.h"
namespace
{
class alpha74: public AlphaBase
{
    public:
    explicit alpha74(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        Num1(cfg->getAttributeIntDefault("para1",30)),
	Num2(cfg->getAttributeIntDefault("para2",37)),
	Num3(cfg->getAttributeIntDefault("para3",15)),
	Num4(cfg->getAttributeIntDefault("para4",11))

    {
    	adv30.resize (GLOBAL::Dates.size());

    	for (int di = 0; di< GLOBAL::Dates.size(); ++di)
    	{
    		adv30[di].resize(GLOBAL::Instruments.size(),nan);
    	}

    	hv.resize (GLOBAL::Dates.size());

    	for (int di = 0; di< GLOBAL::Dates.size(); ++di)
    	{
    		hv[di].resize(GLOBAL::Instruments.size(),nan);
    	}
    	
    	vol_rank.resize (GLOBAL::Dates.size());

    	for (int di = 0; di< GLOBAL::Dates.size(); ++di)
    	{
    		vol_rank[di].resize(GLOBAL::Instruments.size(),nan);
    	}
    }
    void generate(int di) override
    {
	

	    vector<float> rank1(GLOBAL::Instruments.size(),nan);
	    vector<float> rank2(GLOBAL::Instruments.size(),nan);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);
                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv30[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }
       
       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                hv[di-delay][ii] = high[di-delay][ii]*0.0261661+vwap[di-delay][ii]*(1-0.0261661);
            }
        }


       

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vol_rank[di-delay][ii] = vol[di-delay][ii];
            }
        }



		for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
	            vector<float> c1(Num3,nan);                
	            vector<float> c3(Num2,nan);
				vector<float> c4(Num3,nan);
            	for (int i =0; i<Num3;++i){
				c1[i]=close[di-delay-i][ii];

				for (int k=0; k<Num2; ++k){
					c3[k] = adv30[di-delay-k][ii];
			}
			c4[i]=QL_Oputils::sum(c3);
				
			}
			rank1[ii] = QL_Oputils::corr(c1,c4);
            }
            
        }
        QL_Oputils::rank(rank1);


	
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

		vector<float> c5(Num4,nan);
		vector<float> c7(Num4,nan);

		 for (int i =0; i<Num4; ++i){ 
		 	//QL_Oputils::rank(hv[di-delay-i]);
		 	//QL_Oputils::rank(vol_rank[di-delay-i]);
		 	c5[i]= hv[di-delay-i][ii];
		 	c7[i]= vol_rank[di-delay-i][ii];
		 }

		
		 rank2[ii] = QL_Oputils::corr(c5,c7);
		}}

		QL_Oputils::rank(rank2);
		
		for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
				if (rank1[ii]<rank2[ii])
					{alpha[ii]=-1;}
				else 
					{alpha[ii] = 0;}
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
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &low;
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &open;
	const QL_MATRIX<QL_FLOAT> &close;
	vector<vector<float> > adv30;
	vector<vector<float> > hv;
	vector<vector<float> > vol_rank;

        int Num1; 
	int Num2;
	int Num3;
	int Num4;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha74(cfg);
        return str;
    }
}

