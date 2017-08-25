#include "include/qlSim.h"
namespace
{
class alpha67: public AlphaBase
{
    public:
    explicit alpha67(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        sub_indus(dr.getData<QL_MATRIX<QL_INT>>("subindustry")),
        sector(dr.getData<QL_MATRIX<QL_INT>>("sector")),
        Num1(cfg->getAttributeIntDefault("para1",20)),
	Num2(cfg->getAttributeIntDefault("para2",2)),
	Num3(cfg->getAttributeIntDefault("para3",6))

    {
    	adv20.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv20[di].resize(GLOBAL::Instruments.size(),nan);
        }

    	adv_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv_ind[di].resize(GLOBAL::Instruments.size(),nan);
        }    	

        vwap_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vwap_ind[di].resize(GLOBAL::Instruments.size(),nan);
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
                adv20[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	adv_ind[di-delay][ii] = adv20[di-delay][ii]; 
            	vwap_ind[di-delay][ii] = vwap[di-delay][ii];
            }
        }

        for (int i = di-delay-Num3; i< di; ++i)
        {       
        	QL_Oputils::industryNeutralize(adv_ind[di-delay-i],sub_indus[di-delay-i]);
        	QL_Oputils::industryNeutralize(vwap_ind[di-delay-i],sector[di-delay-i]);
        }
     

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {	
            	vector<float> c1(Num2,nan);

            	for (int i =0; i<Num2;++i){
            		c1[i] = high[di-delay-i][ii];
            	}
				rank1[ii] = high[di-delay][ii] - QL_Oputils::llv(c1.begin(),c1.end());
            }
        }

        QL_Oputils::rank(rank1);


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
		
				vector<float> c6(Num3,nan);
				vector<float> c8(Num3,nan);

				for (int c=0; c<Num3; ++c){
				c6[c]= adv_ind[di-delay-c][ii];	
				c8[c]=vwap_ind[di-delay-c][ii];
			}

			rank2[ii] = QL_Oputils::corr(c6,c8);
			}
		}

		QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
 				alpha[ii]= -1 * powf(float(rank1[ii]),float(rank2[ii]));	    		
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
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &vwap;
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_INT> &sub_indus;
	const QL_MATRIX<QL_INT> &sector;
    vector<vector<float> > adv20;
    vector<vector<float> > adv_ind;
    vector<vector<float> > vwap_ind;

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
        AlphaBase * str = new alpha67(cfg);
        return str;
    }
}

