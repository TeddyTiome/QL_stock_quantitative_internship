#include "include/qlSim.h"
namespace
{
class t2: public AlphaBase
{
    public:
    explicit t2(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        hs_c(dr.getData<QL_VECTOR<QL_FLOAT>>("HS300.change")),
        Num1(cfg->getAttributeIntDefault("para1",10)),
	    Num2(cfg->getAttributeIntDefault("para2",10)),
        Num3(cfg->getAttributeIntDefault("para3",12)),
        Num4(cfg->getAttributeIntDefault("para4",10))

    {

        cor_dp.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            cor_dp[di].resize(GLOBAL::Instruments.size(),nan);
        }

        cor_gg.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            cor_gg[di].resize(GLOBAL::Instruments.size(),nan);
        }        

        cor_de.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            cor_de[di].resize(GLOBAL::Instruments.size(),nan);
        }

        vol_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vol_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }             
        

        vwap_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vwap_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }             
        
        clop.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            clop[di].resize(GLOBAL::Instruments.size(),nan);
        }       


        
    }
    void generate(int di) override
    {

			vector<float> rank1(GLOBAL::Instruments.size(),nan);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                clop[di-delay][ii] = close[di-delay][ii]-open[di-delay][ii]; 
                vol_rank[di-delay][ii] = vol[di-delay][ii];
                vwap_rank[di-delay][ii] = vwap[di-delay][ii];
            }
        }

        QL_Oputils::rank(vol_rank[di-delay]);
        QL_Oputils::rank(vwap_rank[di-delay]);

        QL_Oputils::rank(clop[di-delay]);


       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num1,nan);
                vector<float> c2(Num1,nan);

                for (int i = 0; i< Num1;++i)
                {
                    c1[i] = clop[di-delay-i][ii];
                    c2[i] = hs_c[di-delay-i];
                }
                cor_dp[di-delay][ii] = QL_Oputils::corr(c1,c2);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num2,nan);
                vector<float> c2(Num2,nan);

                for (int i = 0; i< Num2;++i)
                {
                    c1[i] = vol_rank[di-delay-i][ii];
                    c2[i] = vwap[di-delay-i][ii];
                }
                cor_gg[di-delay][ii] = QL_Oputils::corr(c1,c2);
            }
        }


       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num4,nan);

                cor_de[di-delay][ii] = 0;
                for (int i = 0; i< Num4;++i)
                {
                    c1[i] = cor_dp[di-delay-i][ii];
                    cor_de[di-delay][ii] += c1[i] * (Num4-i) / (Num4 * (Num4 +1) * 0.5);
                }
                
            }
        }

            QL_Oputils::rank(cor_de[di-delay]);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num4,nan);

                for (int i = 0; i< Num1;++i)
                {
                    c1[i] = cor_dp[di-delay-i][ii];
                }
                QL_Oputils::rank(c1);
                rank1[ii] = c1[0];
            }
        }

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                alpha[ii] = -1 * powf(fabs(cor_gg[di-delay][ii] * cor_de[di-delay][ii]),(rank1[ii])) * clop[di-delay][ii];

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
            ar & cor_dp;
            ar & cor_gg;
            ar & cor_de;
            ar & vol_rank;
            ar & vwap_rank;
            ar & clop;
        }
	const QL_MATRIX<QL_FLOAT> &vol;
    const QL_MATRIX<QL_FLOAT> &close;
    const QL_MATRIX<QL_FLOAT> &open;
    const QL_MATRIX<QL_FLOAT> &vwap;
    const QL_VECTOR<QL_FLOAT> &hs_c;
    vector<vector<float> > cor_dp;
    vector<vector<float> > cor_gg;
    vector<vector<float> > cor_de;
    vector<vector<float> > vol_rank;
    vector<vector<float> > vwap_rank;
    vector<vector<float> > clop;

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
        AlphaBase * str = new t2(cfg);
        return str;
    }
}

