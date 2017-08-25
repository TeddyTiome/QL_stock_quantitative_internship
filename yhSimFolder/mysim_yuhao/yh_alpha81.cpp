#include "include/qlSim.h"
namespace
{
class alpha81: public AlphaBase
{
    public:
    explicit alpha81(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        Num1(cfg->getAttributeIntDefault("para1",10)),
	Num2(cfg->getAttributeIntDefault("para2",49)),
	Num3(cfg->getAttributeIntDefault("para3",8)),
	Num4(cfg->getAttributeIntDefault("para4",14)),
	Num5(cfg->getAttributeIntDefault("para5",5))
    {
    	adv10.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv10[di].resize(GLOBAL::Instruments.size(),nan);
        }

        vwap_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vwap_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }

        vol_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vol_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }          

        pro.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            pro[di].resize(GLOBAL::Instruments.size(),nan);
        }       


    }
    void generate(int di) override
    {
			vector<float> rank1(GLOBAL::Instruments.size(), nan);
			vector<float> rank2(GLOBAL::Instruments.size(), nan);
	
       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);

                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv10[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vwap_rank[di-delay][ii] = vwap[di-delay][ii];
                vol_rank[di-delay][ii] =vol[di-delay][ii];
            }
        }


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                
				vector<float> c6(Num5,nan);
				vector<float> c7(Num5,nan);

				for (int i=0; i<Num5; ++i){
					//QL_Oputils::rank(vwap_rank[di-delay-i]);
					//QL_Oputils::rank(vol_rank[di-delay-i]);

					c6[i]=vwap_rank[di-delay-i][ii];
					c7[i]=vol_rank[di-delay-i][ii];
				}

				rank2[ii] = QL_Oputils::corr(c6,c7);

	    	}
        }

        QL_Oputils::rank(rank2);
		
		for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {        
        		vector<float> c2(Num2,nan);
				vector<float> c4(Num3,nan);
				vector<float> c3(Num3,nan);

			for (int i= 0; i<Num3; ++i){

			for (int j=0; j<Num2;++j){
				c2[j]= adv10[di-delay-j-i][ii];
			}
			c3[i] = QL_Oputils::sum(c2);
			c4[i] = vwap[di-delay-i][ii];
		}
		   	pro[di-delay][ii] = QL_Oputils::corr(c3,c4);
		
			}
		}

		// for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
  //       {
  //           if((valid[ di ][ ii ]))
  //           {  
  //           	QL_Oputils::rank(pro[di-delay]);
  //           	pro[di-delay][ii] = powf(float(pro[di-delay][ii]),4.0);
  //           }
  //       }


		for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {   
            	rank1[ii] = 1.0;
            	for (int m=0; m<Num4; ++m){
            		//QL_Oputils::rank(pro[di-delay-m]);
					rank1[ii] = rank1[ii] * pro[di-delay-m][ii];
				}
				if (!(rank1[ii]<0.0000001))
				{rank1[ii] = log(rank1[ii]);}
				else {rank1[ii] = 0;}
				}
		}

		QL_Oputils::rank(rank1);


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

				if (rank1[ii]<rank2[ii])
					{alpha[ii]=-1;}
				else 
					{alpha[ii]=0;}

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
	vector<vector<float> > adv10;
    vector<vector<float> > vwap_rank;
    vector<vector<float> > vol_rank;
    vector<vector<float> > pro;

        int Num1; 
	int Num2;
	int Num3;
	int Num4;
	int Num5;

};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha81(cfg);
        return str;
    }
}

